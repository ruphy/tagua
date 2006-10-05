/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "chainreaction.h"
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

class ChainReactionAnimator;

class ChainReactionPiece {
public:
  typedef PieceColor Color;
  typedef int Type;
private:
  Color m_color;
  int   m_type;
public:
  ChainReactionPiece()
  : m_color(static_cast<Color>(-1)), m_type(static_cast<Type>(-1)) { }
  ChainReactionPiece(Color color, Type t)
  : m_color(color), m_type(t) { }
  Color color() const { return m_color; }
  Type type() const { return m_type; }
  int id() const { return static_cast<int>(m_color); }
  QString name() const {
                   return (m_color == WHITE ? "white_"
                        : m_color == BLACK ? "black_"
                        : m_color == 2 ? "neutral_" : "unknown")
                      +(m_type>0 ? QString::number(m_type) : QString("add")); }
  static Color oppositeColor(Color color) {
    return color == WHITE ? BLACK : WHITE;
  }
  static int getType(const QString&) { return -1; }
  static QString typeSymbol(int) { return QString(); }

  bool operator==(const ChainReactionPiece& other) const {
    return m_color == other.m_color && m_type == other.m_type;
  }
  bool operator!=(const ChainReactionPiece& other) const {
    return !(*this == other);
  }
  bool operator<(const ChainReactionPiece& other) const {
    return m_color < other.m_color || (m_color == other.m_color && m_type < other.m_type);
  }
  bool equals(const ChainReactionPiece* other) const {
    return other && *this == *other;
  }
  operator bool() const {
    return m_type >= 1 && m_color != -1;
  }
};

typedef Point ChainReactionMove;

class ChainReactionPosition {
public:
  typedef ChainReactionPiece Piece;
  typedef ChainReactionMove Move;
  typedef std::map<Piece, int> Pool;
private:
  Pool m_pool;
  Grid<Piece> m_board;
  Piece::Color m_turn;

public:
  static bool explodeStep(const Grid<Piece>& board, Piece::Color turn, Grid<Piece>& target);

  ChainReactionPosition();
  ChainReactionPosition(const OptList& l);
  ChainReactionPosition(Piece::Color turn, bool wk, bool wq,
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
  QString fen(int,int) const { return QString(); }

  static Move getVerboseMove(int turn, const VerboseNotation& m);
  Move getMove(const AlgebraicNotation& m, bool& ok) const;

  bool testMove(Move& p) const;
  void move(const Move& p);

  Point size() const { return m_board.getSize(); }
  bool valid(const Point& p) const { return m_board.valid(p); }
  void setup();

  const Piece* get(const Point& p) const {
    return valid(p) && m_board[p] ? &m_board[p] : 0;
  }
  Piece* get(const Point& p) {
    return valid(p) && m_board[p] ? &m_board[p] : 0;
  }
  const Piece* operator[](const Point& p) const { return get(p); }
  void set(const Point& p, Piece* piece) {
    if (!valid(p))
      return;
    m_board[p] = piece ? *piece : Piece();
  }
  void removePiece(const Point& p) { set(p, 0); }

  bool operator==(const ChainReactionPosition& other) const;

  shared_ptr<Piece> moveHint(const Move& m) const;

  void dump() const { }
};

ChainReactionPosition::ChainReactionPosition()
: m_board(9,9)
, m_turn(WHITE) { }

ChainReactionPosition::ChainReactionPosition(const OptList& l)
: m_board(options_list_find<IntOpt>(l, "width", 9),
           options_list_find<IntOpt>(l, "height", 9))
, m_turn(WHITE) { }

ChainReactionPosition::ChainReactionPosition(Piece::Color turn, bool, bool, bool, bool, const Point&)
: m_board(9,9)
, m_turn(turn) { }

ChainReactionMove ChainReactionPosition::getMove(const AlgebraicNotation& m, bool& ok) const {
  ok = true;
  return m.to;
}

ChainReactionMove ChainReactionPosition::getVerboseMove(int /*turn*/, const VerboseNotation& m) {
  return m.to;
}

void ChainReactionPosition::setup() {
  for(Point p = m_board.first(); p <= m_board.last(); p = m_board.next(p)) {
    m_board[p] = Piece(static_cast<Piece::Color>(2), static_cast<Piece::Type>(1) );
  }
}

bool ChainReactionPosition::explodeStep(const Grid<Piece>& board, Piece::Color turn, Grid<Piece>& target) {
  bool all_this_color = true;
  for(Point p = board.first(); p <= board.last(); p = board.next(p))
  if(target[p].color() != turn) {
    all_this_color = false;
    break;
  }
  if(all_this_color)
    return false;

  target = board;
  bool exploding = false;

  for(Point p = board.first(); p <= board.last(); p = board.next(p)) {
    int max = 0;
    Point pts[] = { p+Point(0,-1), p+Point(0,1), p+Point(1,0), p+Point(-1,0) };
    for(int i=0;i<4;i++)
    if(board.valid(pts[i]))
      max++;
    if(board[p].type()>max) {
      exploding = true;
      for(int i=0;i<4;i++)
      if(board.valid(pts[i]))
        target[pts[i]] = Piece(turn, static_cast<Piece::Type>(target[pts[i]].type()+1));
      target[p] = Piece(turn, static_cast<Piece::Type>(target[p].type()-max));
    }
  }

  return exploding;
}

void ChainReactionPosition::switchTurn() {
  m_turn = Piece::oppositeColor(m_turn);
}

bool ChainReactionPosition::testMove(Move& p) const {
  if (!valid(p) || m_board[p].color()==Piece::oppositeColor(m_turn))
    return false;

  return true;
}

void ChainReactionPosition::move(const Move& p) {
  m_board[p] = Piece(m_turn, static_cast<Piece::Type>(m_board[p].type()+1));
  Grid<Piece> tmp = m_board;
  while(explodeStep(m_board, m_turn, tmp))
    m_board = tmp;
  switchTurn();
}

bool ChainReactionPosition::operator==(const ChainReactionPosition& other) const {
  return m_board == other.m_board
      && m_turn == other.m_turn;
}

shared_ptr<ChainReactionPiece> ChainReactionPosition::moveHint(const Move& /*p*/) const {
  return shared_ptr<Piece>(new Piece(m_turn, static_cast<Piece::Type>(-1)));
}


//BEGIN ChainReactionAnimator ---------------------------------------------------------------------

class ChainReactionAnimator {
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
  typedef ChainReactionPiece Piece;
  typedef ChainReactionPosition Position;

  PointConverter* m_converter;
  GraphicalPosition* m_position;
  Random m_random;

  bool m_anim_movement;
  bool m_anim_explode;
  bool m_anim_fade;
  bool m_anim_rotate;
public:
  ChainReactionAnimator(PointConverter* converter, GraphicalPosition* position);
  AnimationPtr warp(AbstractPosition::Ptr);
  AnimationPtr forward(AbstractPosition::Ptr, const ChainReactionMove& move);
  AnimationPtr back(AbstractPosition::Ptr, const ChainReactionMove& move);
};

ChainReactionAnimator::ChainReactionAnimator(PointConverter* converter, GraphicalPosition* position)
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

ChainReactionAnimator::AnimationPtr ChainReactionAnimator::warp(AbstractPosition::Ptr final) {
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
          a = shared_ptr<Animation>(new PromotionAnimation( sprite,
                                m_position->setPiece(i, q, false, false) ));
        }
        else {
          // remove it
          m_position->removeElement(i);
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

ChainReactionAnimator::AnimationPtr ChainReactionAnimator::forward(AbstractPosition::Ptr final,
                                                                const ChainReactionMove& m) {

  AnimationPtr retv(new AnimationGroup);
  AnimationPtr curr = retv;

  Grid<Piece> board(final->size().x,final->size().y);
  for (Point i = m_position->first(); i <= m_position->last(); i = m_position->next(i)) {
    Element p = m_position->getElement(i);
    if(p.piece())
      board[i] = Piece(static_cast<Piece::Color>(p.piece()->color()),
                              static_cast<Piece::Type>(p.piece()->type()));
  }
  Grid<unsigned char> exploded(board.getSize().x, board.getSize().y);
  Grid<Piece> target = board;
  Piece::Color turn = Piece::oppositeColor(static_cast<Piece::Color>(final->turn()));
  target[m] = Piece(turn, static_cast<Piece::Type>(target[m].type()+1));

  do {
    bool added = false;

    for(Point i = board.first(); i <= board.last(); i = board.next(i)) {
      int max = 0;
      Point pts[] = { i+Point(0,-1), i+Point(0,1), i+Point(1,0), i+Point(-1,0) };
      for(int j=0;j<4;j++)
      if(board.valid(pts[j]))
        max++;
      exploded[i] = false;
      if(board[i].type() > max) {
        if(shared_ptr<PieceSprite> sprite = m_position->getSprite(i)) {
          curr->addPreAnimation(shared_ptr<Animation>(
            new ExplodeAnimation(sprite, m_random)));
          exploded[i] = true;
        }
      }
    }

//     {AnimationPtr next(new AnimationGroup);
//     curr->addPostAnimation(next);
//     curr = next;}

    for(Point i = target.first(); i <= target.last(); i = target.next(i)) {
      Element p = m_position->getElement(i);
      if(!p.piece() || p.piece()->color() != target[i].color()
                  || p.piece()->type() != target[i].type()) {
        if(p.sprite() && !exploded[i])
          curr->addPreAnimation(shared_ptr<Animation>(
            new CaptureAnimation(p.sprite())));
        AbstractPiece::Ptr piece = ChainReactionVariant::info()->createPiece(
                                              target[i].color(), target[i].type() );
        shared_ptr<PieceSprite> s = m_position->setPiece(i, piece, false, false);
        curr->addPreAnimation(shared_ptr<Animation>(
            new DropAnimation(s)));
        added = true;
      }
    }

    board = target;

#if 1
    AnimationPtr delay(new AnimationGroup);
    delay->addPreAnimation(shared_ptr<Animation>(new DelayAnimation(200)));
    curr->addPreAnimation(delay);
    curr = delay;
#endif
    AnimationPtr next(new AnimationGroup);
    curr->addPostAnimation(next);
    curr = next;
  }
  while(Position::explodeStep(board, turn, target));

  curr->addPostAnimation(warp(final));

  return retv;
}

ChainReactionAnimator::AnimationPtr ChainReactionAnimator::back(AbstractPosition::Ptr final,
                                                                const ChainReactionMove&) {
  return warp(final);
}

//END ChainReactionAnimator -----------------------------------------------------------------------

class ChainReactionVariantInfo {
public:
  typedef ChainReactionPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef ChainReactionAnimator Animator;

  static const bool m_simple_moves = true;
  static const char *m_name;
  static const char *m_theme_proxy;

  static void forallPieces(PieceFunction& f);
  static int moveListLayout() { return 0; }
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h" << "i" << "j" << "k" << "l"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8" << "9" << "10" << "11" << "12";
  }
  static OptList positionOptions() { return OptList()
      << OptPtr(new IntOpt("width", "Board width:", 9, 2, 40))
      << OptPtr(new IntOpt("height", "Board height:", 9, 2, 40)); }
};

const char *ChainReactionVariantInfo::m_name = "ChainReaction";
const char *ChainReactionVariantInfo::m_theme_proxy = "ChainReaction";

void ChainReactionVariantInfo::forallPieces(PieceFunction& f) {
  f(WHITE, 0);
  f(BLACK, 0);
  f(WHITE, 1);
  f(BLACK, 1);
  f(WHITE, 2);
  f(BLACK, 2);
  f(WHITE, 3);
  f(BLACK, 3);
  f(WHITE, 4);
  f(BLACK, 4);
}

VariantInfo* ChainReactionVariant::static_chainreaction_variant = 0;

VariantInfo* ChainReactionVariant::info() {
  if (!static_chainreaction_variant)
    static_chainreaction_variant = new WrappedVariantInfo<ChainReactionVariantInfo>;
  return static_chainreaction_variant;
}

template <>
struct MoveFactory<ChainReactionVariantInfo> {
  static ChainReactionMove createNormalMove(const NormalUserMove& move) {
    return move.to;
  }
  static ChainReactionMove createDropMove(const DropUserMove& move) {
    return move.m_to;
  }
  static NormalUserMove toNormal(const ChainReactionMove& m) {
    return NormalUserMove(Point::invalid(), m);
  }
};

template <>
class MoveSerializer<ChainReactionPosition> : public AbstractMoveSerializer {
  ChainReactionMove m_move;
  const ChainReactionPosition& m_ref;
public:
  MoveSerializer(const ChainReactionMove& m, const ChainReactionPosition& ref)
  : m_move(m), m_ref(ref) { }

  virtual QString SAN() const {
    return m_move.toString(m_ref.size().y);
  }
};

