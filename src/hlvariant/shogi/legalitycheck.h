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
  
  bool legal(Move& move) const;
  
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
bool LegalityCheck<GameState>::legal(Move&) const {
  return true; // BROKEN
}

template <typename GameState>
InteractionType LegalityCheck<GameState>::movable(const TurnTest&, const Point&) const {
  return Moving;
}

template <typename GameState>
InteractionType LegalityCheck<GameState>::droppable(const TurnTest&, int) const {
  return Moving;
}

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__LEGALITYCHECK_H

