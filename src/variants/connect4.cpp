/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "connect4.h"
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
#include "xchess/animator.impl.h"

using namespace boost;

class Connect4Animator;

class Connect4Piece {
public:
  typedef PieceColor Color;
  typedef int Type;
private:
  Color m_color;
public:
  Connect4Piece(Color color, Type)
  : m_color(color) { }
  Connect4Piece(Color color)
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

  bool operator==(const Connect4Piece& other) const {
    return m_color == other.m_color;
  }
  bool operator<(const Connect4Piece& other) const {
    return m_color < other.m_color;
  }
  bool equals(const Connect4Piece* other) const {
    return other && *this == *other;
  }
};

typedef Point Connect4Move;

class Connect4Position {
public:
  typedef Connect4Piece Piece;
  typedef Connect4Move Move;
  typedef std::map<Piece, int> Pool;
private:
  Pool m_pool;
  PointerGrid<Piece> m_board;
  Piece::Color m_turn;

public:
  Connect4Position();
  Connect4Position(const OptList&);
  Connect4Position(Piece::Color turn, bool wk, bool wq,
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

  bool operator==(const Connect4Position& other) const;

  shared_ptr<Piece> moveHint(const Move& m) const;
  void dump() const { }
  QStringList borderCoords() const;
};

Connect4Position::Connect4Position()
: m_board(7,7)
, m_turn(WHITE) { }

Connect4Position::Connect4Position(const OptList&)
: m_board(7,7)
, m_turn(WHITE) { }

Connect4Position::Connect4Position(Piece::Color turn, bool, bool, bool, bool, const Point&)
: m_board(7,7)
, m_turn(turn) { }

QStringList Connect4Position::borderCoords() const
{
  QStringList retv;
  Point p = m_board.getSize();
  for(int i=0; i<p.x; i++)
    retv << QChar('a'+i);
  for(int i=1; i<=p.y; i++)
    retv += QString::number(i);
  return retv + retv;
}

Connect4Move Connect4Position::getMove(const AlgebraicNotation& m, bool& ok) const {
  ok = true;
  return m.to;
}

Connect4Move Connect4Position::getVerboseMove(int /*turn*/, const VerboseNotation& m) {
  return m.to;
}

void Connect4Position::setup() {
}

void Connect4Position::switchTurn() {
  m_turn = Piece::oppositeColor(m_turn);
}

bool Connect4Position::testMove(Move& p) const {
  if (!valid(p) || p.y != 0 || m_board[Point(p.x,1)]) return false;

  return true;
}

void Connect4Position::move(const Move& p) {
  int x = p.x;
  for(int i=6;i>=1;i--)
  if(!m_board[Point(x,i)]) {
    set(Point(x,i), new Piece(m_turn));
    switchTurn();
    return;
  }
}

bool Connect4Position::operator==(const Connect4Position& other) const {
  return m_board == other.m_board
      && m_turn == other.m_turn;
}

shared_ptr<Connect4Piece> Connect4Position::moveHint(const Connect4Move& /*m*/) const {
  return shared_ptr<Piece>(new Piece(m_turn));
}

class Connect4VariantInfo {
public:
  typedef Connect4Position Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef Connect4Animator Animator;

  static const bool m_simple_moves = true;
  static const char *m_name;
  static const char *m_theme_proxy;

  static void forallPieces(PieceFunction& f);
  static int moveListLayout() { return 0; }
  static OptList positionOptions() { return OptList(); }
};

const char *Connect4VariantInfo::m_name = "Connect4";
const char *Connect4VariantInfo::m_theme_proxy = "Connect4";

void Connect4VariantInfo::forallPieces(PieceFunction& f) {
  f(WHITE, -1);
  f(BLACK, -1);
}

VariantInfo* Connect4Variant::static_connect4_variant = 0;

VariantInfo* Connect4Variant::info() {
  if (!static_connect4_variant)
    static_connect4_variant = new WrappedVariantInfo<Connect4VariantInfo>;
  return static_connect4_variant;
}

template <>
struct MoveFactory<Connect4VariantInfo> {
  static Connect4Move createNormalMove(const NormalUserMove& move) {
    return move.to;
  }
  static Connect4Move createDropMove(const Connect4Piece& /*piece*/, const Point& to) {
    return to;
  }
  static NormalUserMove toNormal(const Connect4Move& m) {
    return NormalUserMove(Point::invalid(), m);
  }
};

template <>
class MoveSerializer<Connect4Position> : public AbstractMoveSerializer {
  Connect4Move m_move;
  const Connect4Position& m_ref;
public:
  MoveSerializer(const Connect4Move& m, const Connect4Position& ref)
  : m_move(m), m_ref(ref) { }

  DecoratedMove toDecoratedMove() const {
    return DecoratedMove() << SAN();
  }

  virtual QString SAN() const {
    return m_move.toString(m_ref.size().y);
  }
};


class Connect4Animator : public SimpleAnimator<Connect4VariantInfo> {
  typedef SimpleAnimator<Connect4VariantInfo> Base;
  typedef Base::Position Position;
  typedef Base::Move Move;
  typedef Base::GPosition GPosition;
public:
  Connect4Animator(PointConverter* converter, const boost::shared_ptr<GPosition>& position)
  : Base(converter, position) { }

  AnimationPtr forward(const Position& final, const Move&) {
    return warp(final);
  }
  
  AnimationPtr back(const Position& final, const Move&) {
    return warp(final);
  }
};

