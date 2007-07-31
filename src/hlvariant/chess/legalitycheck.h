#ifndef HLVARIANT__CHESS__LEGALITYCHECK_H
#define HLVARIANT__CHESS__LEGALITYCHECK_H

#include "gamestate.h"
#include "interactiontype.h"
#include "turnpolicy.h"

namespace HLVariant {
namespace Chess {

template <typename _GameState>
class LegalityCheck {
public:
  typedef _GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename GameState::Move Move;
  typedef typename GameState::Piece Piece;
private:
  const GameState& m_state;
public:
  LegalityCheck(const GameState& state);
  virtual ~LegalityCheck();

  virtual typename Move::Type getMoveType(
            const Piece& piece, 
            const Move& move, 
            const Piece& target = Piece()) const;
  virtual bool pseudolegal(Move& move) const;
  virtual bool legal(Move& move) const;
  virtual bool attacks(
            typename Piece::Color color, 
            const Point& p,
            const Piece& target = Piece()) const;
  virtual bool checkPromotion(typename Piece::Type type) const;
  
  virtual InteractionType movable(const TurnTest&, const Point& x) const;
  virtual InteractionType droppable(const TurnTest&, int index) const;
};

// IMPLEMENTATION

template <typename GameState>
LegalityCheck<GameState>::~LegalityCheck() { }

template <typename GameState>
LegalityCheck<GameState>::LegalityCheck(const GameState& state)
: m_state(state) { }

template <typename GameState>
bool LegalityCheck<GameState>::legal(Move& move) const {
  if (pseudolegal(move)) {
    Piece piece = m_state.board().get(move.from());
    typename Piece::Color turn = piece.color();
    
    GameState tmp(m_state);
    tmp.move(move);
    
    Point kingPos = tmp.board().find(Piece(turn, Piece::KING));
    
    if (kingPos == Point::invalid())
      return false;
    
    LegalityCheck<GameState> tmpLegality(tmp);
    if (tmpLegality.attacks(Piece::oppositeColor(turn), kingPos))
      return false;
      
    return true;
  }
  else {
    return false;
  }
}

template <typename GameState>
bool LegalityCheck<GameState>::pseudolegal(Move& move) const {
  if (!move.valid()) return false;

  if (!m_state.board().valid(move.from())) return false;
  if (!m_state.board().valid(move.to())) return false;
  
  Piece piece = m_state.board().get(move.from());
  if (piece == Piece()) return false;
  
  typename Piece::Color thisTurn = piece.color();
  typename Piece::Color otherTurn = Piece::oppositeColor(thisTurn);
  
  if (piece != Piece() && m_state.turn() == thisTurn) {
    typename Move::Type move_type = getMoveType(piece, move);
    move.setType(move_type);
    if (!move.valid())
      return false;
    if (!checkPromotion(static_cast<typename Piece::Type>(move.promoteTo())))
      return false;
    if (move.kingSideCastling()) {
      if (attacks(otherTurn, move.from()) ||
          attacks(otherTurn, move.from() + Point(1, 0), piece))
          return false;
    }
    if (move.queenSideCastling()) {
      if (attacks(otherTurn, move.from()) ||
          attacks(otherTurn, move.from() + Point(-1, 0), piece))
          return false;
    }

    return true;
  }
  else {
    return false;
  }
}

template <typename GameState>
typename GameState::Move::Type 
LegalityCheck<GameState>::
getMoveType(const Piece& piece, const Move& move, const Piece& _target) const {
  Piece target = _target == Piece() ? m_state.board().get(move.to()) : _target;

  switch (piece.type())
  {
  case Piece::ROOK:
  {
      if (move.from() == move.to())
          return Move::INVALID;
      PathInfo path = m_state.board().path(move.from(), move.to());
      if (path.parallel() && path.clear() && target.color() != piece.color())
          return Move::NORMAL;
      else
          return Move::INVALID;
  }

  case Piece::BISHOP:
  {
      if (move.from() == move.to())
          return Move::INVALID;
      PathInfo path = m_state.board().path(move.from(), move.to());
      if (path.diagonal() && path.clear() && target.color() != piece.color())
          return Move::NORMAL;
      else
          return Move::INVALID;
  }

  case Piece::KNIGHT:
      if (target.color() == piece.color())
          return Move::INVALID;
      else
      {
          Point delta = move.from() - move.to();
          if (abs(delta.x) == 1 && abs(delta.y) == 2)
              return Move::NORMAL;
          if (abs(delta.y) == 1 && abs(delta.x) == 2)
              return Move::NORMAL;
          return Move::INVALID;
      }

  case Piece::QUEEN:
  {
      if (move.from() == move.to())
          return Move::INVALID;
      PathInfo path = m_state.board().path(move.from(), move.to());
      if (path.valid() && path.clear() && target.color() != piece.color())
          return Move::NORMAL;
      else
          return Move::INVALID;
  }

  case Piece::KING:
  {
    if (move.from() == move.to())
      return Move::INVALID;
    Point delta = move.to() - move.from();
    if (abs(delta.x) <= 1 && abs(delta.y) <= 1 && target.color() != piece.color()) {
      return Move::NORMAL;
    }
    else if (move.from() == m_state.kingStartingPosition(piece.color())) {
      if (delta == Point(2,0)) {
        if (m_state.board().get(move.from() + Point(1,0)) == Piece() &&
            m_state.board().get(move.to()) == Piece() &&
            m_state.kingCastling(piece.color()))
            return Move::KING_SIDE_CASTLING;
      }
      else if (delta == Point(-2,0)) {
        if (m_state.board().get(move.from() - Point(1, 0)) == Piece() &&
            m_state.board().get(move.to() + Point(1, 0)) == Piece() &&
            m_state.board().get(move.to()) == Piece() &&
            m_state.queenCastling(piece.color()))
            return Move::QUEEN_SIDE_CASTLING;
      }
    }
    return Move::INVALID;
  }

  case Piece::PAWN:
  {
    Point delta = move.to() - move.from();
    bool enPassant = m_state.enPassant() == move.to();

    // moving
    if (target == Piece() && !enPassant) {
      if (delta == m_state.direction(piece.color())) {
        if (move.to().y == m_state.promotionRank(piece.color()))
          return Move::PROMOTION;
        else
          return Move::NORMAL;
      }
      
      if (move.from().y == m_state.startingRank(piece.color()) 
                           + m_state.direction(piece.color()).y &&
          delta == m_state.direction(piece.color()) * 2 &&
          m_state.board().get(move.from() + m_state.direction(piece.color())) == Piece())
        return Move::EN_PASSANT_TRIGGER;
      else
        return Move::INVALID;
    }

    // capturing
    else if (enPassant || target.color() != piece.color()) {
      if (delta.y == m_state.direction(piece.color()).y && 
          abs(delta.x) == 1) {
        if (enPassant)
          return Move::EN_PASSANT_CAPTURE;
        else if (move.to().y == m_state.promotionRank(piece.color()))
          return Move::PROMOTION;
        else
          return Move::NORMAL;
      }
    }

    return Move::INVALID;
  }

  default:
    return Move::INVALID;
  }
}

template <typename GameState>
bool LegalityCheck<GameState>::attacks(typename Piece::Color color, const Point& to, const Piece& target) const {
  for (int i = 0; i < m_state.board().size().x; i++) {
    for (int j = 0; j < m_state.board().size().y; j++) {
      Point p(i, j);
      Piece piece = m_state.board().get(p);
      Move move(p, to);
      if (piece != Piece() && piece.color() == color 
          && getMoveType(piece, move, target) != Move::INVALID)
        return true;
    }
  }
  return false;
}

template <typename GameState>
bool LegalityCheck<GameState>::checkPromotion(typename Piece::Type type) const {
  return type == -1 ||
    type == Piece::QUEEN ||
    type == Piece::ROOK ||
    type == Piece::BISHOP ||
    type == Piece::KNIGHT;
}

template <typename GameState>
InteractionType LegalityCheck<GameState>::movable(const TurnTest& test, const Point& p) const {
  Piece piece = m_state.board().get(p);
  if (piece == Piece() || !test(piece.color()))
    return NoAction;
    
  return piece.color() == m_state.turn() ? Moving : Premoving;
}

template <typename GameState>
InteractionType LegalityCheck<GameState>::droppable(const TurnTest& test, int index) const {
  if (!test(index))
    return NoAction;
  
  typename Piece::Color c = static_cast<typename Piece::Color>(index);
  return c == m_state.turn() ? Moving : Premoving;
}

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__LEGALITYCHECK_H

