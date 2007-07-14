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
#include "kboard_wrapped.h"
#include "animation.h"
#include "animationfactory.h"
#include "moveserializer.impl.h"

class CrazyhouseVariantInfo {
public:
  typedef CrazyhousePosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef Position::PoolReference Pool;

  typedef class CrazyhouseAnimator Animator;
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
  if (move.m_drop.valid() )
    return move.m_drop.color();
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
  return PoolConstReference(&m_pool.find(c)->second, c);
}

CrazyhousePosition::Pool& CrazyhousePosition::rawPool() {
  return m_pool;
}

const CrazyhousePosition::Pool& CrazyhousePosition::rawPool() const {
  return m_pool;
}

bool CrazyhousePosition::pseudolegal(Move& move) const {

  if(!move.m_drop.valid() && move.m_pool != -1 && move.m_pool_index != -1) {
    move.m_drop = pool(move.m_pool).get(move.m_pool_index);
    dump();
    std::cout << move.m_drop << " " << move.m_pool << " " << move.m_pool_index << " " <<
       pool(move.m_pool).size() << std::endl;
  }

  if (move.m_drop.valid()) {
    Q_ASSERT(valid(move.to));

    // cannot drop on occupied squares
    if (m_board[move.to]) return false;

    // cannot drop pawns in first or eighth rank
    if (move.m_drop.type() == PAWN &&
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
  if (move.m_drop.valid()) {
    Q_ASSERT(m_pool[move.m_drop.color()].count(move.m_drop.type()));
    Q_ASSERT(!m_board[move.to]);

    basicDropPiece(new Piece(move.m_drop), move.to);
    if(!--m_pool[move.m_drop.color()][move.m_drop.type()])
      m_pool[move.m_drop.color()].erase(move.m_drop.type());
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

    if ( !m_pool.count(piece.color()) || !m_pool.find(piece.color())->second.count(piece.type()) ) {
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
  return m_pool == pos.m_pool && Base::operator==(pos);
}

//END CrazyhousePosition





typedef UnwrappedGraphicalAPI<CrazyhouseVariantInfo> CrazyhouseGraphicalAPI;

class CrazyhouseAnimator {
  CrazyhouseGraphicalAPI::Ptr m_cinterface;
public:
  CrazyhouseAnimator(CrazyhouseGraphicalAPI::Ptr cinterface)
    : m_cinterface(cinterface) {
  }

  void updatePool(const CrazyhousePosition& final) {
    final.dump();

    for(int color = 0; color < 2; color++) {
      CrazyhousePosition::Color c = static_cast<CrazyhousePosition::Color>(color);
      const CrazyhousePosition::PlayerPool& before = const_cast<CrazyhousePosition*>(
                                                      m_cinterface->position())->rawPool()[c];
      const CrazyhousePosition::PlayerPool& after = const_cast<CrazyhousePosition*>(
                                                                    &final)->rawPool()[c];
      CrazyhousePosition::PlayerPool::const_iterator before_it = before.begin();
      CrazyhousePosition::PlayerPool::const_iterator after_it = after.begin();

      int pos = 0;

      //oh, a nice bunch of write-only magic shit
      while(before_it != before.end() || after_it != after.end()) {
        if(after_it == after.end() || (before_it != before.end()
                && before_it->first < after_it->first )) {
          for(int i=0;i<before_it->second;i++)
            m_cinterface->takePoolSprite(color, pos);
          ++before_it;
        }
        else if (before_it == before.end() || (after_it != after.end()
                && after_it->first < before_it->first )) {
          for(int i=0;i<after_it->second;i++)
            m_cinterface->insertPoolPiece(color, pos, CrazyhousePiece(c, after_it->first) );
          pos += after_it->second;
          ++after_it;
        }
        else {
          Q_ASSERT(after_it->first == before_it->first);
          if(before_it->second < after_it->second)
          for(int i=0;i<after_it->second - before_it->second;i++)
            m_cinterface->insertPoolPiece(color, pos, CrazyhousePiece(c, after_it->first) );
          else if(before_it->second > after_it->second)
          for(int i=0;i<before_it->second - after_it->second;i++)
            m_cinterface->takePoolSprite(color, pos);
          pos += after_it->second;
          ++after_it;
          ++before_it;
        }
      }
#if 0
      while(oldit != curr->end() || newit != pool->end()) {
        if(newit == pool->end() || (oldit != curr->end()
                && oldit->first->less(newit->first) )) {
          removeFromPool(oldit->first, oldit->second);
          ++oldit;
        }
        else if (oldit == curr->end() || (newit != pool->end()
                && newit->first->less(oldit->first) )) {
          addToPool(newit->first, newit->second);
          ++newit;
        }
        else {
          Q_ASSERT(newit->first->equals(oldit->first));
          if(oldit->second < newit->second)
            addToPool(newit->first, newit->second - oldit->second);
          else if(oldit->second > newit->second)
            removeFromPool(newit->first, oldit->second - newit->second);
          ++newit;
          ++oldit;
        }
      }
#endif
    }
  }

  AnimationGroupPtr warp(const CrazyhousePosition& final) {
    const CrazyhousePosition* current = m_cinterface->position();
    AnimationFactory res(m_cinterface->inner());

    for (Point i = current->first(); i <= current->last(); i = current->next(i)) {
      CrazyhousePiece c = current->get(i);
      CrazyhousePiece f = final.get(i);

      if( !c && f ) {
        NamedSprite sprite = m_cinterface->setPiece(i, f, false);
        res.addPreAnimation(Animate::appear(sprite), Animate::Instant);
      }
      else if (c && !f) {
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        res.addPreAnimation(Animate::disappear(old_sprite), Animate::Instant);
      }
      else if(c && f && !(c == f) ) {
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        NamedSprite sprite = m_cinterface->setPiece(i, f, false);
        res.addPreAnimation(Animate::morph(old_sprite, sprite), Animate::Instant);
      }
    }

    updatePool(final);
    return res;
  }

  boost::shared_ptr<AnimationGroup> forward(const CrazyhousePosition& final, const CrazyhouseMove& move) {
    AnimationFactory res(m_cinterface->inner());

    if(move.m_drop) {
      std::pair<int, int> dropped = m_cinterface->droppedPoolPiece();
      if(dropped.first != -1 && dropped.second != -1
          && const_cast<CrazyhousePosition*>(m_cinterface->position())
              ->pool(dropped.first).get(dropped.second) == move.m_drop) {
        NamedSprite drop = m_cinterface->takePoolSprite(dropped.first, dropped.second);
        m_cinterface->setSprite(move.to, drop);
        res.addPreAnimation(Animate::move(drop, move.to));
        return res;
      }
      else {
        NamedSprite drop = m_cinterface->setPiece(move.to, move.m_drop, false);
        res.addPreAnimation(Animate::appear(drop));
      }
    }
    else {
      NamedSprite piece = m_cinterface->takeSprite(move.from);
      NamedSprite captured = m_cinterface->takeSprite(move.to);
      m_cinterface->setSprite(move.to, piece);

      if (piece)
        res.addPreAnimation(Animate::move(piece, move.to));
      else
        ERROR("Bug!!!");

      if (captured)
        res.addPostAnimation(Animate::destroy(captured));

      if (move.type() == CrazyhouseMove::EnPassantCapture) {
        Point phantom(move.to.x, move.from.y);
        NamedSprite capturedPawn = m_cinterface->takeSprite(phantom);

        if (capturedPawn) {
          QPoint real = m_cinterface->converter()->toReal(phantom);
          res.addPostAnimation(Animate::disappear(capturedPawn));
        }
        else
          ERROR("Bug!!!");
      }
      else if (move.type() == CrazyhouseMove::Promotion) {
        CrazyhousePiece promoted = final.get(move.to);

        if (promoted) {
          QPoint real = m_cinterface->converter()->toReal(move.to);
          NamedSprite old_sprite = m_cinterface->getSprite(move.to);
          NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, /*false,*/ false);

          res.addPostAnimation(Animate::morph(old_sprite, new_sprite));
        }
        else
          ERROR("Bug!!!");
      }
      else if (move.type() == CrazyhouseMove::KingSideCastling) {
        Point rookSquare = move.to + Point(1,0);
        Point rookDestination = move.from + Point(1,0);

        NamedSprite rook = m_cinterface->takeSprite(rookSquare);
        m_cinterface->setSprite(rookDestination, rook);
        res.addPreAnimation(Animate::move(rook, rookDestination));
      }
      else if (move.type() == CrazyhouseMove::QueenSideCastling) {
        Point rookSquare = move.to + Point(-2,0);
        Point rookDestination = move.from + Point(-1,0);

        NamedSprite rook = m_cinterface->takeSprite(rookSquare);
        m_cinterface->setSprite(rookDestination, rook);
        res.addPreAnimation(Animate::move(rook, rookDestination));
      }
    }

    updatePool(final);
    return res;
  }

  boost::shared_ptr<AnimationGroup> back(const CrazyhousePosition& final, const CrazyhouseMove& move) {
    AnimationFactory res(m_cinterface->inner());

    if(move.m_drop) {
      NamedSprite drop = m_cinterface->takeSprite(move.to);
      res.addPostAnimation(Animate::destroy(drop));
    }
    else {
      NamedSprite piece = m_cinterface->takeSprite(move.to);
      NamedSprite captured;
      if (CrazyhousePiece captured_piece = final.get(move.to)) {
        captured = m_cinterface->setPiece(move.to, captured_piece, false);
        res.addPreAnimation(Animate::appear(captured));
      }

      if (!piece) {
        piece = m_cinterface->createPiece(move.to, final.get(move.from), false);
        res.addPreAnimation(Animate::appear(piece));
      }

      m_cinterface->setSprite(move.from, piece);


      if (move.type() == CrazyhouseMove::EnPassantCapture) {
        Point phantom(move.to.x, move.from.y);

        if (CrazyhousePiece pawn_piece = final.get(phantom)) {
          NamedSprite captured_pawn = m_cinterface->setPiece(phantom, pawn_piece, false);
          res.addPreAnimation(Animate::appear(captured_pawn));
        }
      }
      else if (move.type() == CrazyhouseMove::Promotion) {
        CrazyhousePiece pawn_piece = final.get(move.from);
        if (pawn_piece) {
          NamedSprite pawn = m_cinterface->createPiece(move.to, pawn_piece, false);
          res.addPreAnimation(Animate::morph(piece, pawn));

          // replace piece with pawn
          m_cinterface->setSprite(move.from, pawn);
          piece = pawn;
        }
      }
      else if (move.type() == CrazyhouseMove::KingSideCastling) {
        Point rookSquare = move.to + Point(1,0);
        Point rookDestination = move.from + Point(1,0);

        NamedSprite rook = m_cinterface->takeSprite(rookDestination);
        m_cinterface->setSprite(rookSquare, rook);

        res.addPreAnimation(Animate::move(rook, rookSquare));
      }
      else if (move.type() == CrazyhouseMove::QueenSideCastling) {
        Point rookSquare = move.to + Point(-2,0);
        Point rookDestination = move.from + Point(-1,0);

        NamedSprite rook = m_cinterface->takeSprite(rookDestination);
        m_cinterface->setSprite(rookSquare, rook);

        res.addPreAnimation(Animate::move(rook, rookSquare));
      }

      res.addPreAnimation(Animate::move(piece, move.from));
    }
    updatePool(final);
    return res;
  }
};




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
