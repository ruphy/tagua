/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOGI__LEGALITYCHECK_H
#define HLVARIANT__SHOGI__LEGALITYCHECK_H

#include "interactiontype.h"
#include "turnpolicy.h"

namespace HLVariant {
namespace Shogi {

template <typename _GameState>
class LegalityCheck {
public:
  typedef _GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  typedef typename GameState::Move Move;
private:
  const GameState& m_state;
public:
  LegalityCheck(const GameState& state);
  virtual ~LegalityCheck();
  
  virtual bool getMoveType(const Piece& piece, const Move& move) const;
  bool legal(Move& move) const;
  bool pseudolegal(Move& move) const;
  
  virtual InteractionType movable(const TurnTest&, const Point& x) const;
  virtual InteractionType droppable(const TurnTest&, int index) const;
};

// IMPLEMENTATION

template <typename GameState>
LegalityCheck<GameState>::LegalityCheck(const GameState& state)
: m_state(state) { }

template <typename GameState>
LegalityCheck<GameState>::~LegalityCheck() { }

template <typename GameState>
bool LegalityCheck<GameState>::getMoveType(const Piece& piece, const Move& move) const {
  if (!move.valid())
    return false;
  
  if (move.from() == move.to()) 
    return false;
    
  if (m_state.board().get(move.to()).color() == piece.color()) 
    return false;
  Point delta = move.to() - move.from();

  if (!piece.promoted()) {
    switch (piece.type()) {
    case Piece::KING:
      return abs(delta.x) <= 1 && abs(delta.y) <= 1;
    case Piece::GOLD:
      return (delta.x == 0 && abs(delta.y) == 1)
          || (delta.y == 0 && abs(delta.x) == 1)
          || (delta.y == m_state.direction(piece.color()).y && abs(delta.x) <= 1);
    case Piece::SILVER:
      return (abs(delta.x) == abs(delta.y) && abs(delta.x) == 1)
          || (delta.y == m_state.direction(piece.color()).y && abs(delta.x) <= 1);
    case Piece::ROOK:
      {
        PathInfo path = m_state.board().path(move.from(), move.to());
        return path.parallel() && path.clear();
      }
    case Piece::BISHOP:
      {
          PathInfo path = m_state.board().path(move.from(), move.to());
          return path.diagonal() && path.clear();
      }
    case Piece::KNIGHT:
      {
        return abs(delta.x) == 1 && delta.y == m_state.direction(piece.color()).y * 2;
      }
    case Piece::LANCE:
      {
        PathInfo path = m_state.board().path(move.from(), move.to());
        return delta.x == 0 && 
               path.clear() && 
               (delta.y * m_state.direction(piece.color()).y > 0);
      }
    case Piece::PAWN:
      return delta == m_state.direction(piece.color());
    default:
      return false;
    }
  }
  else {
    switch (piece.type()) {
    case Piece::SILVER:
    case Piece::PAWN:
    case Piece::LANCE:
    case Piece::KNIGHT:
      return (delta.x == 0 && abs(delta.y) == 1)
          || (delta.y == 0 && abs(delta.x) == 1)
          || (delta.y == m_state.direction(piece.color()).y && abs(delta.x) <= 1);
    case Piece::ROOK:
      {
        if (abs(delta.x) <= 1 && abs(delta.y) <= 1) return true;
        PathInfo path = m_state.board().path(move.from(), move.to());
        return path.parallel() && path.clear();
      }
    case Piece::BISHOP:
      {
          if (abs(delta.x) <= 1 && abs(delta.y) <= 1) return true;
          PathInfo path = m_state.board().path(move.from(), move.to());
          return path.diagonal() && path.clear();
      }
    default:
      return false;
    }
  }
}

template <typename GameState>
bool LegalityCheck<GameState>::pseudolegal(Move& move) const {
  if (move.drop() == Piece() && 
      move.pool() != -1 && 
      move.index() != -1) {
    move.setDrop(m_state.pools().pool(move.pool()).get(move.index()));
  }

  Piece dropped = move.drop();
  if (dropped != Piece()) {
    if (m_state.board().get(move.to()) != Piece())
      return false;

//     if (stuckPiece(dropped, m.to)) return false; // BROKEN

    if (dropped.type() == Piece::PAWN) {
      for (int i = 0; i < m_state.board().size().y; i++) {
        Piece other = m_state.board().get(Point(move.to().x, i));
        if (other.type() == Piece::PAWN &&
            other.color() == m_state.turn() &&
            !other.promoted())
          return false;
      }
    }
    
    return true;
  }
  else {
    Piece piece = m_state.board().get(move.from());
    if (piece != Piece() && getMoveType(piece, move)) {
      if (piece.type() == Piece::PAWN &&
          !piece.promoted() &&
          (m_state.promotionZone(piece.color(), move.to()) ||
            m_state.promotionZone(piece.color(), move.from())))
        move.setType(Move::PROMOTION);
      return true;
    }
    else
      return false;
  }
}

template <typename GameState>
bool LegalityCheck<GameState>::legal(Move& move) const {
  if (!pseudolegal(move))
    return false;

  GameState tmp(m_state);
  tmp.move(move);

  // find king position
  Point king_pos = tmp.board().find(Piece(m_state.turn(), Piece::KING));
  if (!king_pos.valid()) 
    return false;

  // check if the king can be captured
  for (int i = 0; i < m_state.board().size().x; i++) {
    for (int j = 0; j < m_state.board().size().y; j++) {
      Point p(i, j);
      Piece piece = tmp.board().get(p);
      if (piece.color() == tmp.turn() && getMoveType(piece, Move(p, king_pos)))
        return false;
    }
  }

  return true;
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

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__LEGALITYCHECK_H

