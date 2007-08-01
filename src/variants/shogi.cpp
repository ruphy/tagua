/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "shogi.h"
#include "xchess/animator.h"
#include "xchess/piece.h"
#include "xchess/move.h"
#include "moveserializer.impl.h"
#include "xchess/dropanimator.impl.h"
#include "crazyhouse_p.h"
#include "interactiontype.h"
#include "turnpolicy.h"
#include "tagua_wrapped.h"
#include "icsapi.impl.h"

class ShogiPiece {
public:
  enum Type {
    KING,
    GOLD,
    SILVER,
    KNIGHT,
    LANCE,
    ROOK,
    BISHOP,
    PAWN,
    INVALID_TYPE
  };
  enum Color {
    BLACK,
    WHITE,
    INVALID_COLOR
  };
  typedef bool PromotionType;
private:
  Color m_color;
  Type m_type;
  bool m_promoted;
public:
  ShogiPiece()
  : m_color(INVALID_COLOR)
  , m_type(INVALID_TYPE)
  , m_promoted(false) { }
  ShogiPiece(ShogiPiece::Color color, ShogiPiece::Type type, bool promoted = false);
  ShogiPiece(const ShogiPiece& other);

  void promote() { m_promoted = true; }
  bool promoted() { return m_promoted; }

  bool operator<(const ShogiPiece& p) const {
    if (m_promoted == p.m_promoted)
      if (m_color == p.m_color)
        return m_type < p.m_type;
      else
        return m_color < p.m_color;
    else
      return m_promoted < p.m_promoted;
  }

  QString name() const;
  static QString typeName(ShogiPiece::Type t);
  bool valid() const { return m_color != INVALID_COLOR && m_type != INVALID_TYPE; }
  operator bool() const { return valid(); }
  bool operator!() const { return !valid(); }

  bool equals(const ShogiPiece* p) const {
    if (valid()) {
      if (p)
        return (*this) == (*p);
      else
        return false;
    }
    else
      return !p;
  }

  bool operator==(const ShogiPiece& p) const {
    return m_promoted == p.m_promoted &&
           m_color == p.m_color &&
           m_type == p.m_type;
  }

  bool operator!=(const ShogiPiece& p) const {
    return !(operator==(p));
  }

  static Type getType(const QString& t);
  static QString typeSymbol(ShogiPiece::Type t);

  bool canMove(const class ShogiPosition&, const Point&, const Point&) const;
  Color color() const { return m_color; }
  Type type() const { return m_type; }

  static Color oppositeColor(Color c) { return c == WHITE ? BLACK : WHITE; }
  Point direction() const { return Point(0, m_color == WHITE ? 1 : -1); }
};

ShogiPiece::ShogiPiece(ShogiPiece::Color color, ShogiPiece::Type type, bool promoted)
: m_color(color)
, m_type(type)
, m_promoted(promoted) { }

ShogiPiece::ShogiPiece(const ShogiPiece& other)
: m_color(other.m_color)
, m_type(other.m_type)
, m_promoted(other.m_promoted) { }

QString ShogiPiece::name() const {
  QString res = m_color == WHITE ? "white_" : "black_";
  if (m_promoted)
    res += "p_";
  res += typeName(m_type);
  return res;
}

QString ShogiPiece::typeName(ShogiPiece::Type t) {
  switch (t) {
  case KING:
    return "king";
  case GOLD:
    return "gold";
  case SILVER:
    return "silver";
  case KNIGHT:
    return "knight";
  case LANCE:
    return "lance";
  case ROOK:
    return "rook";
  case BISHOP:
    return "bishop";
  case PAWN:
    return "pawn";
  default:
    return "unknown";
  }
}

ShogiPiece::Type ShogiPiece::getType(const QString&) {
  return KING; // FIXME
}

QString ShogiPiece::typeSymbol(ShogiPiece::Type t) {
  switch (t) {
  case KING:
    return "K";
  case GOLD:
    return "G";
  case SILVER:
    return "S";
  case KNIGHT:
    return "N";
  case LANCE:
    return "L";
  case ROOK:
    return "R";
  case BISHOP:
    return "B";
  case PAWN:
    return "P";
  default:
    return "?";
  }
}

// ------------------------------

class ShogiMove {
  ShogiPiece m_drop;

  int m_pool;
  int m_pool_index;

  bool m_promote;
  template<typename T> friend class MoveSerializer;
public:
  Point from;
  Point to;

  ShogiMove();
  ShogiMove(const Point& from, const Point& to, bool promote);
  ShogiMove(int pool, int pool_index, const Point& to);

  QString toString(int) const;

  bool operator==(const ShogiMove& other) const;

  const ShogiPiece& drop() const { return m_drop; }
  void setDrop(const ShogiPiece& piece) { m_drop = piece; }
  int pool() const { return m_pool; }
  int poolIndex() const { return m_pool_index; }

  bool promote() const { return m_promote; }
  bool valid() const { return to.valid(); }
};

ShogiMove::ShogiMove()
: m_pool(-1)
, m_pool_index(-1)
, m_promote(true)
, from(Point::invalid())
, to(Point::invalid()) { }

ShogiMove::ShogiMove(const Point& from, const Point& to, bool promote)
: m_pool(-1)
, m_pool_index(-1)
, m_promote(promote)
, from(from)
, to(to) { }

ShogiMove::ShogiMove(int pool, int pool_index, const Point& to)
: m_pool(pool)
, m_pool_index(pool_index)
, m_promote(false)
, from(Point::invalid())
, to(to) { }

QString ShogiMove::toString(int) const {
  return "";
}

bool ShogiMove::operator==(const ShogiMove& other) const {
  if (m_drop)
    return m_drop == other.m_drop
        && to == other.to;
  else
    return m_promote == other.m_promote
        && to == other.to
        && from == other.from;
}

// ------------------------------

class ShogiPosition {
public:
  typedef ShogiPiece Piece;
  typedef ShogiMove Move;

  typedef PoolReference<ShogiPosition> PoolReference;
  typedef PoolConstReference<ShogiPosition> PoolConstReference;
  typedef PoolReference::Pool Pool;
  typedef PoolReference::PlayerPool PlayerPool;
private:
  Piece::Color m_turn;
  Grid<Piece> m_board;
  Pool m_pool;
public:
  template<typename T> friend class MoveSerializer;
  ShogiPosition();
  ShogiPosition(const ShogiPosition&);
  ShogiPosition(Piece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  ShogiPosition(const QList<boost::shared_ptr<BaseOpt> >& opts);
  virtual ShogiPosition* clone() const { return new ShogiPosition(*this); }
  virtual ~ShogiPosition() { }

  virtual void setup();

  bool testMove(Move&) const;
  bool pseudolegal(Move& m) const;

  PoolReference pool(int);
  PoolConstReference pool(int) const;

  PlayerPool& rawPool(Piece::Color color) { return m_pool[color]; }
  const PlayerPool& rawPool(Piece::Color color) const { return const_cast<Pool&>(m_pool)[color]; }

  virtual const Point first() const { return m_board.first(); }
  virtual const Point last() const { return m_board.last(); }
  virtual Point next(const Point& p) const { return m_board.next(p); }
  inline bool valid(const Point& p) const { return m_board.valid(p); }

  ShogiPiece get(const Point& p) const;
  void set(const Point& p, const Piece& piece);

  ShogiPiece operator[](const Point& p) const { return m_board[p]; }

  Piece::Color turn() const { return m_turn; }
  void setTurn(Piece::Color turn) { m_turn = turn; }
  Piece::Color previousTurn() const { return Piece::oppositeColor(m_turn); }
  void switchTurn() { m_turn = Piece::oppositeColor(m_turn); }

  InteractionType movable(const TurnTest& test, const Point& p) const {
    if (!valid(p) || !m_board[p] || !test(m_board[p].color()))
      return NoAction;
    return m_board[p].color() == m_turn ? Moving : Premoving;
  }
  InteractionType droppable(const TurnTest& test, int p) const {
    if (!test(m_turn))
      return NoAction;
    ShogiPiece::Color c = static_cast<ShogiPiece::Color>(p);
    return c == m_turn ? Moving : Premoving;
  }
  void move(const ShogiMove& m);

  void fromFEN(const QString&, bool& ok) { ok = false; }
  QString fen(int, int) const { return ""; }
  bool operator==(const ShogiPosition& p) const;

  static Move getVerboseMove(Piece::Color turn, const VerboseNotation& m);
  Move getMove(const AlgebraicNotation&, bool& ok) const;
  boost::shared_ptr<ShogiPiece> moveHint(const ShogiMove& m) const;

  Point size() const { return Point(9,9); }
  void dump() const { }

  static bool promotionZone(Piece::Color color, const Point& p);
  static bool stuckPiece(const Piece& piece, const Point& to);
  PathInfo path(const Point& from, const Point& to) const { return m_board.path(from, to); }
  QStringList borderCoords() const;
};

ShogiPosition::ShogiPosition()
: m_turn(ShogiPiece::BLACK)
, m_board(9,9) { }

ShogiPosition::ShogiPosition(const ShogiPosition& other)
: m_turn(other.m_turn)
, m_board(other.m_board)
, m_pool(other.m_pool) { }

ShogiPosition::ShogiPosition(Piece::Color turn, bool, bool, bool, bool, const Point&)
: m_turn(turn)
, m_board(9, 9) { }

ShogiPosition::ShogiPosition(const QList<boost::shared_ptr<BaseOpt> >&)
: m_turn(ShogiPiece::BLACK)
, m_board(9,9) { }

QStringList ShogiPosition::borderCoords() const
{
  QStringList retv;
  for(int i=m_board.getSize().y; i>0; i--)
    retv += QString::number(i);
  retv << QChar(0x4e5d) << QChar(0x516b) << QChar(0x4e03) << QChar(0x516d)
    << QChar(0x4e94) << QChar(0x56db) << QChar(0x4e09) << QChar(0x4e8c) << QChar(0x4e00);
  return retv + retv;
}

bool ShogiPiece::canMove(const ShogiPosition& pos,
                         const Point& from, const Point& to) const {
  if (!from.valid()) return false;
  if (!to.valid()) return false;
  if (from == to) return false;
  if (pos[to].color() == m_color) return false;
  Point delta = to - from;

  if (!m_promoted) {
    switch (m_type) {
    case KING:
      return abs(delta.x) <= 1 && abs(delta.y) <= 1;
    case GOLD:
      return (delta.x == 0 && abs(delta.y) == 1)
          || (delta.y == 0 && abs(delta.x) == 1)
          || (delta.y == direction().y && abs(delta.x) <= 1);
    case SILVER:
      return (abs(delta.x) == abs(delta.y) && abs(delta.x) == 1)
          || (delta.y == direction().y && abs(delta.x) <= 1);
    case ROOK:
      {
        PathInfo path = pos.path(from, to);
        return path.parallel() && path.clear();
      }
    case BISHOP:
      {
          PathInfo path = pos.path(from, to);
          return path.diagonal() && path.clear();
      }
    case KNIGHT:
      {
        return abs(delta.x) == 1 && delta.y == direction().y * 2;
      }
    case LANCE:
      {
        PathInfo path = pos.path(from, to);
        return delta.x == 0 && path.clear() && (delta.y * direction().y > 0);
      }
    case PAWN:
      return delta.x == 0 && delta.y == direction().y;
    default:
      return false;
    }
  }
  else {
    switch (m_type) {
    case SILVER:
    case PAWN:
    case LANCE:
    case KNIGHT:
      return (delta.x == 0 && abs(delta.y) == 1)
          || (delta.y == 0 && abs(delta.x) == 1)
          || (delta.y == direction().y && abs(delta.x) <= 1);
    case ROOK:
      {
        if (abs(delta.x) <= 1 && abs(delta.y) <= 1) return true;
        PathInfo path = pos.path(from, to);
        return path.parallel() && path.clear();
      }
    case BISHOP:
      {
          if (abs(delta.x) <= 1 && abs(delta.y) <= 1) return true;
          PathInfo path = pos.path(from, to);
          return path.diagonal() && path.clear();
      }
    default:
      return false;
    }
  }
}


ShogiPosition::Move ShogiPosition::getVerboseMove(Piece::Color, const VerboseNotation&) {
  return Move();
}

ShogiPosition::Move ShogiPosition::getMove(const AlgebraicNotation&, bool& ok) const {
  ok = false;
  return Move();
}

ShogiPiece ShogiPosition::get(const Point& p) const {
  if (m_board.valid(p)) {
    return m_board[p];
  }
  else {
    return ShogiPiece();
  }
}

void ShogiPosition::set(const Point& p, const ShogiPiece& piece) {
  if (m_board.valid(p)) {
    m_board[p] = piece;
  }
}

ShogiPosition::PoolReference ShogiPosition::pool(int index) {
  ShogiPiece::Color color = static_cast<ShogiPiece::Color>(index);
  return PoolReference(&m_pool[color], color);
}

ShogiPosition::PoolConstReference ShogiPosition::pool(int index) const {
  ShogiPiece::Color color = static_cast<ShogiPiece::Color>(index);
  return PoolConstReference(&m_pool[color], color);
}

bool ShogiPosition::operator==(const ShogiPosition& p) const {
  return m_turn == p.m_turn
      && m_board == p.m_board;
}

boost::shared_ptr<ShogiPiece> ShogiPosition::moveHint(const ShogiMove& m) const {
  if (m.drop()) return boost::shared_ptr<ShogiPiece>(new ShogiPiece(m.drop()));
  else return boost::shared_ptr<ShogiPiece>();
}

bool ShogiPosition::promotionZone(Piece::Color color, const Point& p) {
  return color == ShogiPiece::WHITE ? p.y >= 6 : p.y <= 2;
}

#define SET_PIECE(i,j, color, type) m_board[Point(i,j)] = Piece(ShogiPiece::color, ShogiPiece::type)
void ShogiPosition::setup() {
  for (int i = 0; i < m_board.getSize().x; i++) {
    SET_PIECE(i, 2, WHITE, PAWN);
    SET_PIECE(i, 6, BLACK, PAWN);
  }

  SET_PIECE(0,0, WHITE, LANCE);
  SET_PIECE(1,0, WHITE, KNIGHT);
  SET_PIECE(2,0, WHITE, SILVER);
  SET_PIECE(3,0, WHITE, GOLD);
  SET_PIECE(4,0, WHITE, KING);
  SET_PIECE(5,0, WHITE, GOLD);
  SET_PIECE(6,0, WHITE, SILVER);
  SET_PIECE(7,0, WHITE, KNIGHT);
  SET_PIECE(8,0, WHITE, LANCE);
  SET_PIECE(1,1, WHITE, ROOK);
  SET_PIECE(7,1, WHITE, BISHOP);

  SET_PIECE(0,8, BLACK, LANCE);
  SET_PIECE(1,8, BLACK, KNIGHT);
  SET_PIECE(2,8, BLACK, SILVER);
  SET_PIECE(3,8, BLACK, GOLD);
  SET_PIECE(4,8, BLACK, KING);
  SET_PIECE(5,8, BLACK, GOLD);
  SET_PIECE(6,8, BLACK, SILVER);
  SET_PIECE(7,8, BLACK, KNIGHT);
  SET_PIECE(8,8, BLACK, LANCE);
  SET_PIECE(1,7, BLACK, BISHOP);
  SET_PIECE(7,7, BLACK, ROOK);


  m_turn = ShogiPiece::BLACK;
}
#undef SET_PIECE

bool ShogiPosition::testMove(Move& m) const {
  if (!pseudolegal(m))
    return false;

  ShogiPosition tmp(*this);
  tmp.move(m);

  // find king position
  Point king_pos = Point::invalid();
  ShogiPiece king(m_turn, ShogiPiece::KING);
  for (Point i = tmp.m_board.first(); i <= tmp.m_board.last(); i = tmp.m_board.next(i)) {
    if (ShogiPiece p = tmp[i])
      if (p == king) {
        king_pos = i;
        break;
      }
  }
  if (!king_pos.valid()) return false;

  // check if the king can be captured
  for (Point i = tmp.m_board.first(); i <= tmp.m_board.last(); i = tmp.m_board.next(i)) {
    if (ShogiPiece p = tmp[i])
      if (p.color() == tmp.turn() && p.canMove(tmp, i, king_pos)) return false;
  }

  return true;
}

bool ShogiPosition::stuckPiece(const ShogiPiece& piece, const Point& p) {
  if (piece.type() == Piece::PAWN) {
    if (p.y == (piece.color() == Piece::WHITE ? 8 : 0))
      return true;
  }
  else if (piece.type() == Piece::KNIGHT) {
    if (piece.color() == Piece::WHITE) {
      if (p.y >= 7)
        return true;
    }
    else {
      if (p.y <= 1)
        return true;
    }
  }
  return false;
}

bool ShogiPosition::pseudolegal(Move& m) const {
  if (!m.drop() && m.pool() != -1 && m.poolIndex() != -1) {
    m.setDrop(pool(m.pool()).get(m.poolIndex()));
  }

  if (ShogiPiece dropped = m.drop()) {
    if (m_board[m.to]) return false;
    if (stuckPiece(dropped, m.to)) return false;
    if (dropped.type() == Piece::PAWN) {
      for (int i = 0; i < m_board.getSize().y; i++)
        if (ShogiPiece other = m_board[Point(m.to.x, i)])
          if (other.color() == m_turn && other.type() == Piece::PAWN && !other.promoted()) return false;
    }
    return true;
  }
  else {
    const Piece& p = m_board[m.from];
    return p && p.canMove(*this, m.from, m.to);
  }
}

void ShogiPosition::move(const ShogiMove& m) {
  if (Piece dropped = m.drop()) {
    m_board[m.to] = dropped;
    if (!--rawPool(dropped.color())[dropped.type()])
      rawPool(dropped.color()).erase(dropped.type());
  }
  else {
    if (Piece captured = m_board[m.to]) {
      rawPool(Piece::oppositeColor(captured.color()))[captured.type()]++;
    }

    m_board[m.to] = m_board[m.from];
    m_board[m.from] = Piece();
  }

  if (promotionZone(m_turn, m.to) || promotionZone(m_turn, m.from)) {
    if (m.promote() || stuckPiece(m_board[m.to], m.to)) {
      Piece::Type type = m_board[m.to].type();
      if (type != ShogiPiece::KING && type != ShogiPiece::GOLD)
        m_board[m.to].promote();
    }
  }

  switchTurn();
}

class ShogiAnimatorBase;

class ShogiVariantInfo {
public:
  static QStringList getNumbers() {
    return QStringList() << QChar(0x4e5d) << QChar(0x516b) << QChar(0x4e03)
              << QChar(0x516d) << QChar(0x4e94) << QChar(0x56db)
              << QChar(0x4e09) << QChar(0x4e8c) << QChar(0x4e00);
  }

  typedef ShogiPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef DropAnimatorMixin<ShogiAnimatorBase> Animator;
  typedef NoPool Pool;
  
  static const bool hasICS = false;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static int moveListLayout() { return 0; }
  static OptList positionOptions() { return OptList(); }
  static const char *m_name;
  static const char *m_theme_proxy;
};

const char *ShogiVariantInfo::m_name = "Shogi";
const char *ShogiVariantInfo::m_theme_proxy = "Shogi";


void ShogiVariantInfo::forallPieces(PieceFunction& f) {
  ChessVariant::forallPieces(f);
}

VariantInfo* ShogiVariant::static_shogi_variant = 0;

VariantInfo* ShogiVariant::info() {
  if (!static_shogi_variant)
    static_shogi_variant = new WrappedVariantInfo<ShogiVariantInfo>;
  return static_shogi_variant;
}

class ShogiAnimatorBase : public BaseAnimator<ShogiVariantInfo> {
public:
  typedef ShogiVariantInfo Variant;
protected:
  typedef BaseAnimator<ShogiVariantInfo> Base;
  typedef Base::API API;
public:
  ShogiAnimatorBase(API cinterface)
  : Base(cinterface) { }

  virtual AnimationGroupPtr forward(const ShogiPosition& final, const ShogiMove& move) {
    AnimationFactory res(m_cinterface->inner());

    NamedSprite piece = m_cinterface->takeSprite(move.from);
    NamedSprite captured = m_cinterface->takeSprite(move.to);
    m_cinterface->setSprite(move.to, piece);

    if (piece)
      res.addPreAnimation(Animate::move(piece, move.to));

    if (captured)
      res.addPostAnimation(Animate::destroy(captured));

    if (final.get(move.to) != m_cinterface->position()->get(move.from)) {
      Piece promoted = final.get(move.to);

      if (promoted) {
        QPoint real = m_cinterface->converter()->toReal(move.to);
        NamedSprite old_sprite = m_cinterface->getSprite(move.to);
        NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, false);

        res.addPostAnimation(Animate::morph(old_sprite, new_sprite));
      }
    }

    return res;
  }

  virtual AnimationGroupPtr back(const ShogiPosition& final, const ShogiMove& move) {
    AnimationFactory res(m_cinterface->inner());

    NamedSprite piece = m_cinterface->takeSprite(move.to);
    NamedSprite captured;
    if (Piece captured_piece = final.get(move.to)) {
      captured = m_cinterface->setPiece(move.to, captured_piece, false);
      res.addPreAnimation(Animate::appear(captured));
    }

    if (!piece) {
      piece = m_cinterface->createPiece(move.to, final.get(move.from), false);
      res.addPreAnimation(Animate::appear(piece));
    }

    m_cinterface->setSprite(move.from, piece);

    if (final.get(move.from) != m_cinterface->position()->get(move.to)) {
      Piece old_piece = final.get(move.from);
      if (old_piece) {
        NamedSprite old = m_cinterface->createPiece(move.to, old_piece, false);
        res.addPreAnimation(Animate::morph(piece, old));

        // replace piece with pawn
        m_cinterface->setSprite(move.from, old);
        piece = old;
      }
    }

    res.addPreAnimation(Animate::move(piece, move.from));

    return res;

  }
};

template <>
class MoveSerializer<ShogiPosition> {
  const ShogiMove&     m_move;
  const ShogiPosition& m_ref;
  bool isAmbiguous() const {
    ShogiPiece p = m_move.drop() ? m_move.drop() : m_ref.m_board[m_move.from];
    bool ambiguous = false;
    if (!m_move.drop())
    for (Point i = m_ref.m_board.first(); i <= m_ref.m_board.last(); i = m_ref.m_board.next(i) ) {
      if (i==m_move.from || m_ref.m_board[i] != p)
        continue;
      ShogiMove mv(i, m_move.to, false);
      if (m_ref.testMove(mv)) {
        ambiguous = true;
        break;
      }
    }
    return ambiguous;
  }
public:
  MoveSerializer(const ShogiMove& m, const ShogiPosition& r)
    : m_move(m), m_ref(r) { }

  QString SAN() const {
    ShogiPiece p = m_move.drop() ? m_move.drop() : m_ref.m_board[m_move.from];
    bool ambiguous = isAmbiguous();
    QString retv;
    if (p.promoted())
      retv += "+";
    retv += ShogiPiece::typeSymbol(p.type());
    if (ambiguous) {
      retv += QString::number(m_ref.m_board.getSize().x-m_move.from.x);
      retv += QString(m_move.from.y+'a');
    }
    if (m_move.drop())
      retv += "*";
    else if (m_ref.m_board[m_move.to])
      retv += "x";
    else
      retv += "-";
    retv += QString::number(m_ref.m_board.getSize().x-m_move.to.x);
    retv += QString(m_move.to.y+'a');
    if (!p.promoted() && !m_move.drop() &&
            ShogiPosition::promotionZone(m_ref.turn(), m_move.to)) {
      if (m_move.m_promote)
        retv += "+";
      else
        retv += "=";
    }
    return retv;
  }

  DecoratedMove toDecoratedMove() const {
    ShogiPiece p = m_move.drop() ? m_move.drop() : m_ref.m_board[m_move.from];
    bool ambiguous = isAmbiguous();
    DecoratedMove retv;
    if(p.type() == ShogiPiece::KING)
      retv.push_back(MovePart(p.color() == ShogiPiece::BLACK?"king1":"king2", MovePart::Figurine));
    else
      retv.push_back(MovePart((p.promoted() ? "p_" : "") + ShogiPiece::typeName(p.type()), MovePart::Figurine));
    if (ambiguous) {
      retv.push_back(MovePart(QString::number(m_ref.m_board.getSize().x-m_move.from.x)));
      retv.push_back(MovePart("num_"+QString::number(m_move.from.y+1), MovePart::Figurine));
    }
    QString mmm;
    if (m_move.drop())
      mmm += "*";
    else if (m_ref.m_board[m_move.to])
      mmm += "x";
    else
      mmm += "-";
    mmm += QString::number(m_ref.m_board.getSize().x-m_move.to.x);
    retv.push_back(mmm);
    retv.push_back(MovePart("num_"+QString::number(m_move.to.y+1), MovePart::Figurine));
    if (!p.promoted() && !m_move.drop() &&
            ShogiPosition::promotionZone(m_ref.turn(), m_move.to)) {
      if (m_move.m_promote)
        retv.push_back(MovePart("+"));
      else
        retv.push_back(MovePart("="));
    }
    return retv;
  }
};


template <>
struct MoveFactory<ShogiVariantInfo> {
  static ShogiMove createNormalMove(const NormalUserMove& move) {
    return ShogiMove(move.from, move.to, move.promotionType >= 0);
  }
  static ShogiMove createDropMove(const DropUserMove& move) {
    return ShogiMove(move.m_pool, move.m_piece_index, move.m_to);
  }

  static NormalUserMove toNormal(const ShogiMove& move) {
    return NormalUserMove(move.from, move.to);
  }
};



