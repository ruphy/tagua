/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "crazyhouse_p.h"
#include "crazyhouse.h"
#include "tagua_wrapped.h"
#include "icsapi.impl.h"
#include "xchess/dropanimator.impl.h"
#include "animation.h"
#include "animationfactory.h"
#include "moveserializer.impl.h"

class CrazyhouseVariantInfo {
public:
  typedef CrazyhousePosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef Position::PoolReference Pool;

  typedef DropAnimatorMixin<SimpleAnimator<CrazyhouseVariantInfo> > Animator;

  static const bool hasICS = true;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static int moveListLayout(){ return 0; }
  static const char *m_name;
  static const char *m_theme_proxy;
  static OptList positionOptions() { return OptList(); }
};

//BEGIN CrazyhousePiece

CrazyhousePiece::CrazyhousePiece(ChessPiece::Color color, ChessPiece::Type type, bool promoted)
: ChessPiece(color, type)
, m_promoted(promoted) { }

CrazyhousePiece::CrazyhousePiece(const CrazyhousePiece& other)
: ChessPiece(other)
, m_promoted(other.m_promoted) { }

CrazyhousePiece::CrazyhousePiece(const ChessPiece& other)
: ChessPiece(other)
, m_promoted(false) { }

//END CrazyhousePiece

//BEGIN CrazyhouseMove

CrazyhouseMove::CrazyhouseMove(const ChessMove& m)
: ChessMove(m)
, m_drop(INVALID_COLOR, INVALID_TYPE)
, m_pool(-1)
, m_pool_index(-1) { }

CrazyhouseMove::CrazyhouseMove(const Point& from, const Point& to, PieceType promotionType)
: ChessMove(from, to, promotionType)
, m_drop(INVALID_COLOR, INVALID_TYPE)
, m_pool(-1)
, m_pool_index(-1) { }

CrazyhouseMove::CrazyhouseMove(const CrazyhouseMove& other)
: ChessMove(other)
, m_drop(other.m_drop)
, m_pool(other.m_pool)
, m_pool_index(other.m_pool_index) { }

CrazyhouseMove::CrazyhouseMove(const CrazyhousePiece& pc, const Point& to)
: ChessMove(Point::invalid(), to)
, m_drop(pc)
, m_pool(-1)
, m_pool_index(-1) { }

CrazyhouseMove::CrazyhouseMove(int pool, int pool_index, const Point& to)
: ChessMove(Point::invalid(), to)
, m_drop(INVALID_COLOR, INVALID_TYPE)
, m_pool(pool)
, m_pool_index(pool_index) { }

//END CrazyhouseMove

//BEGIN CrazyhousePosition

CrazyhousePosition::CrazyhousePosition() {
}

CrazyhousePosition::CrazyhousePosition(const OptList&) {
}

CrazyhousePosition::CrazyhousePosition(const CrazyhousePosition& other)
: Base(other)
, m_pool(other.m_pool) {
}

CrazyhousePosition::CrazyhousePosition(const ChessPosition& other)
: Base(other) {
}

CrazyhousePosition::CrazyhousePosition(CrazyhousePiece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep)
: Base(turn, wk, wq, bk, bq, ep) {
}

CrazyhousePosition* CrazyhousePosition::clone() const {
  return new CrazyhousePosition(*this);
}

CrazyhousePiece::Color CrazyhousePosition::moveTurn(const Move& move) const {
  if (move.drop())
    return move.drop().color();
  else
    return Base::moveTurn(move);
}

boost::shared_ptr<AbstractGenerator<CrazyhouseMove> >
CrazyhousePosition::createLegalGenerator() const {
  return boost::shared_ptr<AbstractGenerator<CrazyhouseMove> >(
    new MoveGenerator<CrazyhousePosition,
      LegalMove<CrazyhousePosition> >(*this));
}

CrazyhousePosition::PoolReference CrazyhousePosition::pool(int index) {
  Color c = static_cast<Color>(index);
  return PoolReference(&m_pool[c], c);
}

CrazyhousePosition::PoolConstReference CrazyhousePosition::pool(int index) const {
  Color c = static_cast<Color>(index);
  return PoolConstReference(&m_pool[c], c);
}

CrazyhousePosition::PlayerPool& CrazyhousePosition::rawPool(Piece::Color color) {
  return m_pool[color];
}

const CrazyhousePosition::PlayerPool& CrazyhousePosition::rawPool(Piece::Color color) const {
  return m_pool[color];
}

bool CrazyhousePosition::pseudolegal(Move& move) const {

  if (!move.drop() && move.pool() != -1 && move.poolIndex() != -1) {
    move.setDrop(pool(move.pool()).get(move.poolIndex()));
    dump();
    std::cout << move.drop() << " " << move.pool() << " " << move.poolIndex() << " " <<
       pool(move.pool()).size() << std::endl;
  }

  if (move.drop()) {
    Q_ASSERT(valid(move.to));

    // cannot drop on occupied squares
    if (m_board[move.to]) return false;

    // cannot drop pawns in first or eighth rank
    if (move.drop().type() == PAWN &&
        (move.to.y == 0 || move.to.y == 7))
      return false;

    return true;
  }
  // normal move
  else
    return Base::pseudolegal(move);
}

void CrazyhousePosition::move(const Move& move) {
  // drop
  if (move.drop()) {
    Q_ASSERT(m_pool[move.drop().color()].count(move.drop().type()));
    Q_ASSERT(!m_board[move.to]);

    basicDropPiece(new Piece(move.drop()), move.to);
    if(!--m_pool[move.drop().color()][move.drop().type()])
      m_pool[move.drop().color()].erase(move.drop().type());
  }
  else {
    // normal move
    Base::move(move);

    // set promoted flag
    if (move.type() == Move::Promotion) {
      m_board[move.to].setPromoted(true);
    }
  }

#if 0
  for(Pool::iterator i = m_pool.begin(); i != m_pool.end(); ++i)
    std::cout << i->first.color() << "." << i->first.type() << " " << i->second <<std::endl;
#endif
}

void CrazyhousePosition::executeCaptureOn(const Point& point) {
  Piece piece = m_board[point];
  if (piece) {
    Piece downgraded( Piece::oppositeColor(piece.color()),
                piece.promoted() ? PAWN : piece.type());
    m_pool[downgraded.color()][downgraded.type()]++;
  }
  Base::executeCaptureOn(point);
}

CrazyhouseMove CrazyhousePosition::getMove(const AlgebraicNotation& san, bool& ok) const {

  if (san.drop) {
    Piece piece(m_turn, (PieceType)san.type);

    if (!m_pool[piece.color()].count(piece.type())) {
      ok = false;
      return CrazyhouseMove::invalid();
    }
    else {
      ok = true;
      return CrazyhouseMove( piece, san.to);
    }
  }
  else
    return Base::getMove(san, ok);
}

void CrazyhousePosition::dump() const {
  Base::dump();

  for(Pool::const_iterator j = m_pool.begin(); j != m_pool.end(); ++j)
  for(PlayerPool::const_iterator i = j->second.begin(); i != j->second.end(); ++i)
    std::cout << j->first << "." << i->first << " " << i->second <<std::endl;
}

bool CrazyhousePosition::operator==(const CrazyhousePosition& pos) const {
  bool base = Base::operator==(pos);
  std::cout << "crazyhouse chessboards equals: " << base << std::endl;
  bool pools = m_pool == pos.m_pool;
  std::cout << "crazyhouse pools equals: " << pools << std::endl;
  if (!pools) {
    // dump pools
    {
      const PlayerPool& pp = rawPool(WHITE);
      for (PlayerPool::const_iterator it = pp.begin();
          it != pp.end(); ++it) {
        std::cout << ChessPiece::typeSymbol(it->first) << ":" << it->second << " ";
      }
    }
    std::cout << " | ";
    {
      const PlayerPool& pp = rawPool(BLACK);
      for (PlayerPool::const_iterator it = pp.begin();
          it != pp.end(); ++it) {
        std::cout << ChessPiece::typeSymbol(it->first) << ":" << it->second << " ";
      }
    }
    std::cout << std::endl;
    
    // dump pools
    {
      const PlayerPool& pp = pos.rawPool(WHITE);
      for (PlayerPool::const_iterator it = pp.begin();
          it != pp.end(); ++it) {
        std::cout << ChessPiece::typeSymbol(it->first) << ":" << it->second << " ";
      }
    }
    std::cout << " | ";
    {
      const PlayerPool& pp = pos.rawPool(BLACK);
      for (PlayerPool::const_iterator it = pp.begin();
          it != pp.end(); ++it) {
        std::cout << ChessPiece::typeSymbol(it->first) << ":" << it->second << " ";
      }
    }
    std::cout << std::endl;
  }
  return pools && base;
}

//END CrazyhousePosition

//BEGIN PieceFactory<CrazyhouseVariantInfo>


// piece factory
template <>
class PieceFactory<CrazyhouseVariantInfo> {
public:
  static CrazyhousePiece createPiece(const QString& description) {
    return ChessPiece::fromDescription(description);
  }
};

//END PieceFactory<CrazyhouseVariantInfo>


//BEGIN CrazyhouseVariant


const char *CrazyhouseVariantInfo::m_name = "Crazyhouse";
const char *CrazyhouseVariantInfo::m_theme_proxy = "Chess";

VariantInfo* CrazyhouseVariant::static_crazyhouse_variant = 0;

void CrazyhouseVariantInfo::forallPieces(PieceFunction& f) {
  return ChessVariant::forallPieces(f);
}

VariantInfo* CrazyhouseVariant::info() {
  if (!static_crazyhouse_variant)
    static_crazyhouse_variant = new WrappedVariantInfo<CrazyhouseVariantInfo>;
  return static_crazyhouse_variant;
}

//END CrazyhouseVariant
