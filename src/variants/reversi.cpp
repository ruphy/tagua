/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "reversi.h"
#include <map>
#include "xchess/animator.h"
#include "xchess/piece.h"
#include "xchess/move.h"
#include "piecefunction.h"
#include "highlevel.h"
#include "algebraicnotation.h"
#include "graphicalposition.h"
#include "pointconverter.h"
#include "piecegrid.h"

using namespace boost;

class ReversiAnimator;

class ReversiPiece {
public:
  typedef PieceColor Color;
  typedef int Type;
private:
  Color m_color;
public:
  ReversiPiece(Color color, Type)
  : m_color(color) { }
  ReversiPiece(Color color)
  : m_color(color) { }
  Color color() const { return m_color; }
  Type type() const { return -1; }
  int id() const { return static_cast<int>(m_color); }
  QString name() const { return m_color == WHITE ? "white_stone"
                               : m_color == BLACK ? "black_stone" : "unknown"; }
  static Color oppositeColor(Color color) {
    return color == WHITE ? BLACK : WHITE;
  }
  static int getType(const QString&) { return -1; }
  static QString typeSymbol(int) { return ""; }

  bool operator==(const ReversiPiece& other) const {
    return m_color == other.m_color;
  }
  bool operator<(const ReversiPiece& other) const {
    return m_color < other.m_color;
  }
  bool equals(const ReversiPiece* other) const {
    return other && *this == *other;
  }
};

typedef Point ReversiMove;

class ReversiPosition {
public:
  typedef ReversiPiece Piece;
  typedef ReversiMove Move;
  typedef std::map<Piece, int> Pool;
private:
  Pool m_pool;
  PointerGrid<Piece> m_board;
  Piece::Color m_turn;

  int checkDirection(const Point& p, const Point& dir) const;
  void flipDirection(Point p, const Point& dir);
  void flip(const Point& p);
public:
  ReversiPosition();
  ReversiPosition(const OptList& l);
  ReversiPosition(Piece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);

  Pool& pool() { return m_pool; }
  const Pool& pool() const { return m_pool; }
  void addToPool(const Piece&, int) { }
  void removeFromPool(const Piece&, int) { }

  Piece::Color turn() const { return m_turn; }
  void setTurn(Piece::Color turn) { m_turn = turn; }
  void switchTurn();
  Piece::Color previousTurn() const { return Piece::oppositeColor(m_turn); }

  void fromFEN(const QString&, bool& ok) { ok = true;}
  QString fen(int,int) const { return ""; }

  static Move getVerboseMove(int turn, const VerboseNotation& m);
  Move getMove(const AlgebraicNotation& m, bool& ok) const;

  bool testMove(Move& p) const;
  void move(const Move& p);

  Point size() const { return m_board.getSize(); }
  bool valid(const Point& p) const { return m_board.valid(p); }
  void setup();

  const Piece* get(const Point& p) const {
    return valid(p) ? m_board[p] : 0;
  }
  Piece* get(const Point& p) {
    return valid(p) ? m_board[p] : 0;
  }
  const Piece* operator[](const Point& p) const { return get(p); }
  void set(const Point& p, Piece* piece) {
    if (!valid(p))
      return;
    delete m_board[p];
    m_board[p] = piece;
  }
  void removePiece(const Point& p) { set(p, 0); }

  bool operator==(const ReversiPosition& other) const;

  shared_ptr<Piece> moveHint(const Move& m) const;
  void dump() const { }
};

ReversiPosition::ReversiPosition()
: m_board(8,8)
, m_turn(WHITE) { }

ReversiPosition::ReversiPosition(const OptList&)
: m_board(8,8)
, m_turn(WHITE) { }

ReversiPosition::ReversiPosition(Piece::Color turn, bool, bool, bool, bool, const Point&)
: m_board(8,8)
, m_turn(turn) { }

ReversiMove ReversiPosition::getMove(const AlgebraicNotation& m, bool& ok) const {
  ok = true;
  return m.to;
}

ReversiMove ReversiPosition::getVerboseMove(int /*turn*/, const VerboseNotation& m) {
  return m.to;
}

void ReversiPosition::setup() {
  for (int i = 3; i <= 4; i++)
  for (int j = 3; j <= 4; j++) {
    m_board[Point(i,j)] = new Piece(
      (i+j) % 2 == 0 ? WHITE : BLACK);
  }
}

void ReversiPosition::switchTurn() {
  m_turn = Piece::oppositeColor(m_turn);
  for (Point p = m_board.first(); p <= m_board.last(); p = m_board.next(p))
    if (testMove(p)) return;
  // no move: restore old turn
  m_turn = Piece::oppositeColor(m_turn);
}

int ReversiPosition::checkDirection(const Point& p, const Point& dir) const {
  Point pos = p + dir;
  int n = 0;

  while (valid(pos) && m_board[pos]) {
    if (m_board[pos]->color() != m_turn)
      n++;
    else return n;
    pos += dir;
  }

  return 0;
}

void ReversiPosition::flipDirection(Point p, const Point& dir) {
  const int n = checkDirection(p, dir);
  for (int i = 0; i < n; i++)
    flip(p += dir);
}

void ReversiPosition::flip(const Point& p) {
  Q_ASSERT(m_board[p]);
  set(p, new Piece(Piece::oppositeColor(m_board[p]->color())));
}

bool ReversiPosition::testMove(Move& p) const {
  if (!valid(p) || m_board[p]) return false;

  for (int i = -1; i <= 1; i++)
  for (int j = -1; j <= 1; j++) {
    if (i == 0 && j == 0) continue;
    if (checkDirection(p, Point(i, j)) > 0) return true;
  }

  return false;
}

void ReversiPosition::move(const Move& p) {
  set(p, new Piece(m_turn));

  for (int i = -1; i <= 1; i++)
  for (int j = -1; j <= 1; j++) {
    if (i == 0 && j == 0) continue;
    flipDirection(p, Point(i, j));
  }

  switchTurn();
}

bool ReversiPosition::operator==(const ReversiPosition& other) const {
  return m_board == other.m_board
      && m_turn == other.m_turn;
}

shared_ptr<ReversiPiece> ReversiPosition::moveHint(const ReversiMove& /*m*/) const {
  return shared_ptr<Piece>(new Piece(m_turn));
}


//BEGIN ReversiAnimator ---------------------------------------------------------------------

class ReversiAnimator {
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;

  PointConverter* m_converter;
  GraphicalPosition* m_position;
  Random m_random;

  bool m_anim_movement;
  bool m_anim_explode;
  bool m_anim_fade;
  bool m_anim_rotate;
public:
  ReversiAnimator(PointConverter* converter, GraphicalPosition* position);
  AnimationPtr warp(AbstractPosition::Ptr);
  AnimationPtr forward(AbstractPosition::Ptr, const ReversiMove& move);
  AnimationPtr back(AbstractPosition::Ptr, const ReversiMove& move);
};

ReversiAnimator::ReversiAnimator(PointConverter* converter, GraphicalPosition* position)
: m_converter(converter)
, m_position(position)
, m_anim_movement(false)
, m_anim_explode(false)
, m_anim_fade(false)
, m_anim_rotate(false) {
  if(position->getBoolSetting("AnimationsEnabled", true)) {
    m_anim_movement = (bool)position->getBoolSetting("AnimateMovement", true);
    m_anim_explode = (bool)position->getBoolSetting("AnimateExplode", true);
    m_anim_fade = (bool)position->getBoolSetting("AnimateFade", true);
    m_anim_rotate = (bool)position->getBoolSetting("AnimateTransform", true);
  }
}

ReversiAnimator::AnimationPtr ReversiAnimator::warp(AbstractPosition::Ptr final) {
  AnimationPtr res(new AnimationGroup);
  for (Point i = m_position->first(); i <= m_position->last(); i = m_position->next(i)) {
    QPoint real = m_converter->toReal(i);
    Element p = m_position->getElement(i);
    AbstractPiece::Ptr q = final->get(i);
    shared_ptr<Animation> a;

    if (p) {
      shared_ptr<PieceSprite> sprite = p.sprite();
      Q_ASSERT(sprite);

      if (!p.piece()->equals(q)) {
        shared_ptr<PieceSprite> sprite = p.sprite();

        if (q) {
          if((m_anim_explode||m_anim_fade) && m_anim_rotate) {
            shared_ptr<PieceSprite> new_sprite = m_position->setPiece(i, q, false, false);
            shared_ptr<AnimationGroup> g(new AnimationGroup);
            g->addPreAnimation(shared_ptr<Animation>(new GrowAnimation(new_sprite)));
            if(0)/*m_anim_explode)*/
              g->addPreAnimation(shared_ptr<Animation>(new ExplodeAnimation(sprite, m_random)));
            else
              g->addPreAnimation(shared_ptr<Animation>(new FadeAnimation(sprite, real, 255, 0)));
            a = g;
          }
          else
            a = shared_ptr<Animation>(new PromotionAnimation( sprite,
                                          m_position->setPiece(i, q, false, false) ));
        }
        else {
          // remove it
          m_position->removeElement(i);
          if(m_anim_fade)
            a = shared_ptr<Animation>(new FadeAnimation(sprite, real, 255, 0));
          else
            a = shared_ptr<Animation>(new CaptureAnimation(sprite));
        }
      }
    }
    else if (q) {
      a = shared_ptr<Animation>(new DropAnimation( m_position->setPiece(i, q, false, false) ));
    }

    if (a) res->addPreAnimation(a);
  }

  return res;
}

ReversiAnimator::AnimationPtr ReversiAnimator::forward(AbstractPosition::Ptr final, const ReversiMove&) {
  return warp(final);
}

ReversiAnimator::AnimationPtr ReversiAnimator::back(AbstractPosition::Ptr final, const ReversiMove&) {
  return warp(final);
}

//END ReversiAnimator -----------------------------------------------------------------------

class ReversiVariantInfo {
public:
  typedef ReversiPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef ReversiAnimator Animator;

  static const bool m_simple_moves = true;
  static const char *m_name;
  static const char *m_theme_proxy;

  static void forallPieces(PieceFunction& f);
  static int moveListLayout() { return 0; }
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
  }
  static OptList positionOptions() { return OptList(); }
};

const char *ReversiVariantInfo::m_name = "Reversi";
const char *ReversiVariantInfo::m_theme_proxy = "Reversi";

void ReversiVariantInfo::forallPieces(PieceFunction& f) {
  f(WHITE, -1);
  f(BLACK, -1);
}

VariantInfo* ReversiVariant::static_reversi_variant = 0;

VariantInfo* ReversiVariant::info() {
  if (!static_reversi_variant)
    static_reversi_variant = new WrappedVariantInfo<ReversiVariantInfo>;
  return static_reversi_variant;
}

template <>
struct MoveFactory<ReversiVariantInfo> {
  static ReversiMove createNormalMove(const NormalUserMove& move) {
    return move.to;
  }
  static ReversiMove createDropMove(const ReversiPiece& p, const Point& to) {
    return to;
  }
  static NormalUserMove toNormal(const ReversiMove& m) {
    return NormalUserMove(Point::invalid(), m);
  }
};

template <>
class MoveSerializer<ReversiPosition> : public AbstractMoveSerializer {
  ReversiMove m_move;
  const ReversiPosition& m_ref;
public:
  MoveSerializer(const ReversiMove& m, const ReversiPosition& ref)
  : m_move(m), m_ref(ref) { }

  virtual QString SAN() const {
    return m_move.toString(m_ref.size().y);
  }
};

