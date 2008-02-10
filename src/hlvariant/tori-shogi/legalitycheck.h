/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__TORISHOGI__LEGALITYCHECK_H
#define HLVARIANT__TORISHOGI__LEGALITYCHECK_H

#include "interactiontype.h"
#include "../shogi/legalitycheck.h"

namespace HLVariant {
namespace ToriShogi {

template <typename _GameState>
class LegalityCheck: public Shogi::LegalityCheck<_GameState> {
  typedef Shogi::LegalityCheck<_GameState> Base;
public:
  typedef _GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  typedef typename GameState::Move Move;
  
  LegalityCheck(const GameState& state);

  virtual bool getMoveType(const Piece& piece, const Move& move) const;
  bool legal(Move& move) const;
  bool pseudolegal(Move& move) const;
protected:
  virtual bool stuckPiece(const Piece& piece, const Point& p) const;
};

// IMPLEMENTATION

template <typename GameState>
LegalityCheck<GameState>::LegalityCheck(const GameState& state)
: Base(state) { }

template <typename GameState>
bool LegalityCheck<GameState>::getMoveType(const Piece& piece, const Move& move) const {
  if (!move.valid())
    return false;
  
  if (move.from() == move.to()) 
    return false;
    
  if (Base::m_state.board().get(move.to()).color() == piece.color()) 
    return false;
  Point delta = move.to() - move.from();

  if (!piece.promoted()) {
    switch (piece.type()) {
    case Piece::PHOENIX:
      return abs(delta.x) <= 1 && abs(delta.y) <= 1;
    case Piece::FALCON:
      return (abs(delta.x) == 1 && abs(delta.y) <= 1)
	|| (delta.x == 0 && delta.y == Base::m_state.direction(piece.color()).y);
    case Piece::CRANE:
      return (abs(delta.y) == 1 && abs(delta.x) <= 1);
    case Piece::PHEASANT:
      return (delta.x == 0 && delta.y == 2 * Base::m_state.direction(piece.color()).y)
	|| (abs(delta.x) == 1 && delta.y == -Base::m_state.direction(piece.color()).y);

    case Piece::SWALLOW:
      return delta == Base::m_state.direction(piece.color());

    case Piece::RIGHT_QUAIL:
      {
	if (delta.x == 1 && delta.y == -Base::m_state.direction(piece.color()).y)
	  return true;
	PathInfo path = Base::m_state.board().path(move.from(), move.to());
	return ((path.parallel() && delta.y * Base::m_state.direction(piece.color()).y > 0) ||
		(path.diagonal() && delta.y * Base::m_state.direction(piece.color()).y < 0
		 && delta.x < 0)) && path.clear();
      }
    case Piece::LEFT_QUAIL:
      {
	if (delta.x == -1 && delta.y == -Base::m_state.direction(piece.color()).y)
	  return true;
	PathInfo path = Base::m_state.board().path(move.from(), move.to());
	return ((path.parallel() && delta.y * Base::m_state.direction(piece.color()).y > 0) ||
		(path.diagonal() && delta.y * Base::m_state.direction(piece.color()).y < 0
		 && delta.x > 0)) && path.clear();
      }

    default:
      return false;
    }
  }
  else {
    switch (piece.type()) {
    case Piece::SWALLOW: // Goose
      return (delta.x == 0 && delta.y == -2 * Base::m_state.direction(piece.color()).y)
	|| (abs(delta.x) == 2 && delta.y == 2 * Base::m_state.direction(piece.color()).y);

    case Piece::FALCON: // Eagle
      {
	if (abs(delta.x) <= 1 && abs(delta.y) <= 1)
	  return true;
	PathInfo path = Base::m_state.board().path(move.from(), move.to());
	return ((path.parallel() && delta.y * Base::m_state.direction(piece.color()).y < 0) ||
		(path.diagonal() && delta.y * Base::m_state.direction(piece.color()).y > 0) ||
		(path.diagonal() && delta.y * Base::m_state.direction(piece.color()).y == -2)) &&
	  path.clear();
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
    move.setDrop(Base::m_state.pools().pool(move.pool()).get(move.index()));
  }

  Piece dropped = move.drop();
  if (dropped != Piece()) {
    if (Base::m_state.board().get(move.to()) != Piece())
      return false;

    if (stuckPiece(dropped, move.to())) 
      return false;

    if (dropped.type() == Piece::SWALLOW) {
      int otherswallows = 0;
      for (int i = 0; i < Base::m_state.board().size().y; i++) {
        Piece other = Base::m_state.board().get(Point(move.to().x, i));
        if (other.type() == Piece::SWALLOW &&
            other.color() == Base::m_state.turn() &&
            !other.promoted()) {
	  otherswallows++;
	  if (otherswallows >= 2)
	    return false;
	}
      }
    }
    
    return true;
  }
  else {
    Piece piece = Base::m_state.board().get(move.from());
    if (piece != Piece() && getMoveType(piece, move)) {
      if (Base::m_state.canPromote(piece) &&
            (Base::m_state.promotionZone(piece.color(), move.to()) ||
            Base::m_state.promotionZone(piece.color(), move.from())))
        move.setType(Move::PROMOTION);
      return true;
    }
    else {
      return false;
    }
  }
}


template <typename GameState>
bool LegalityCheck<GameState>::legal(Move& move) const {
  if (!pseudolegal(move))
    return false;

  GameState tmp(Base::m_state);
  tmp.move(move);

  // find king and prince positions
  Point king_pos = tmp.board().find(Piece(Base::m_state.turn(), Piece::PHOENIX));

  if (!king_pos.valid())
    return false;

  // check if the king can be captured
  if (Base::canBeCaptured(tmp, king_pos))
    return false;

  return true;
}

template <typename GameState>
bool LegalityCheck<GameState>::stuckPiece(const Piece& piece, const Point& p) const {
  if (piece.type() == Piece::SWALLOW) {
    return p.y == Base::m_state.startingRank(Piece::oppositeColor(piece.color()));
  }
  else {
    return false;
  }
}

} // namespace ToriShogi
} // namespace HLVariant

#endif // HLVARIANT__TORISHOGI__LEGALITYCHECK_H
