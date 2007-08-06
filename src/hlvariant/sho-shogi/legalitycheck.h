/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOSHOGI__LEGALITYCHECK_H
#define HLVARIANT__SHOSHOGI__LEGALITYCHECK_H

#include "interactiontype.h"
#include "../shogi/legalitycheck.h"

namespace HLVariant {
namespace ShoShogi {

template <typename _GameState>
class LegalityCheck: public Shogi::LegalityCheck<_GameState> {
  typedef Shogi::LegalityCheck<_GameState> Base;
public:
  typedef _GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  typedef typename GameState::Move Move;
  
  LegalityCheck(const GameState& state);
  bool legal(Move& move) const;
  virtual InteractionType droppable(const TurnTest&, int index) const;
};

// IMPLEMENTATION

template <typename GameState>
LegalityCheck<GameState>::LegalityCheck(const GameState& state)
: Base(state) { }

template <typename GameState>
InteractionType LegalityCheck<GameState>::droppable(const TurnTest&, int) const {
  return NoAction;
}

template <typename GameState>
bool LegalityCheck<GameState>::legal(Move& move) const {
  if (!pseudolegal(move))
    return false;

  GameState tmp(Base::m_state);
  tmp.move(move);

  // find king and prince positions
  Point king_pos = tmp.board().find(Piece(Base::m_state.turn(), Piece::KING));
  Point prince_pos = tmp.board().find(Piece(Base::m_state.turn(), Piece::DRUNKEN_ELEPHANT, true));

  // check if the king and prince can be captured
  if ((canBeCaptured(tmp, king_pos) && canBeCaptured(tmp, prince_pos)) ||
      (canBeCaptured(tmp, king_pos) && !prince_pos.valid()) ||
      (canBeCaptured(tmp, prince_pos) && !king_pos.valid()) ||
      (!prince_pos.valid() && !king_pos.valid()))
    return false;

  return true;
}

} // namespace ShoShogi
} // namespace HLVariant

#endif // HLVARIANT__SHOSHOGI__LEGALITYCHECK_H
