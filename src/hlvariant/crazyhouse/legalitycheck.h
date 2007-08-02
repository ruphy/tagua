/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__LEGALITYCHECK_H
#define HLVARIANT__CRAZYHOUSE__LEGALITYCHECK_H

#include "../chess/legalitycheck.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename _GameState>
class LegalityCheck : public Chess::LegalityCheck<_GameState> {
public:
  typedef _GameState GameState;
private:
  typedef Chess::LegalityCheck<GameState> Base;
  typedef typename Base::Board Board;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
protected:
  using Base::m_state;
public:
  LegalityCheck(const GameState& state);
  
  virtual bool pseudolegal(Move& move) const;
  virtual typename Piece::Color mover(const Move& move) const;
};

// IMPLEMENTATION

template <typename GameState>
LegalityCheck<GameState>::LegalityCheck(const GameState& state)
: Base(state) { }

template <typename GameState>
bool LegalityCheck<GameState>::pseudolegal(Move& move) const {
  // add drop information to move, if missing
  if (move.drop() == Piece() && 
      move.pool() != Piece::INVALID_COLOR && 
      move.index() != -1) {
    move.setDrop(m_state.pools().pool(move.pool()).get(move.index()));
  }

  Piece dropped = move.drop();
  
  if (dropped == Piece()) {
    return Base::pseudolegal(move);
  }
  else {
    // dropping on a valid square
    if (!m_state.board().valid(move.to()))
      return false;

    // cannot drop on occupied squares
    if (m_state.board().get(move.to()) != Piece())
      return false;
      
    // cannot drop pawns in first or eighth rank
    if (dropped.type() == Piece::PAWN &&
          (move.to().y == m_state.startingRank(Piece::WHITE) || 
           move.to().y == m_state.promotionRank(Piece::WHITE)))
      return false;
      
    return true;
  }
}

template <typename GameState>
typename LegalityCheck<GameState>::Piece::Color 
LegalityCheck<GameState>::mover(const Move& move) const {
  if (move.drop() != Piece())
    return move.drop().color();
  else
    return Base::mover(move);
}

} // namespace Crazyhouse
} // namespace HLVariant


#endif // HLVARIANT__CRAZYHOUSE__LEGALITYCHECK_H
