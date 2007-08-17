/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__DUMMY__LEGALITYCHECK_H
#define HLVARIANT__DUMMY__LEGALITYCHECK_H

#include "../chess/legalitycheck.h"
#include "interactiontype.h"
#include "turnpolicy.h"

namespace HLVariant {
namespace Dummy {

template <typename _GameState>
class LegalityCheck {
public:
  typedef _GameState GameState;
protected:
  typedef typename GameState::Board Board;
  typedef typename GameState::Move Move;
  typedef typename Board::Piece Piece;
  
  const GameState& m_state;
public:
  LegalityCheck(const GameState&);
  virtual ~LegalityCheck();
  
  virtual bool legal(Move&) const;
  virtual InteractionType movable(const TurnTest&, const Point&);
  virtual InteractionType droppable(const TurnTest&, int);
};

// IMPLEMENTATION

template <typename GameState>
LegalityCheck<GameState>::LegalityCheck(const GameState& state) 
: m_state(state) { }

template <typename GameState>
LegalityCheck<GameState>::~LegalityCheck() { }

template <typename GameState>
bool LegalityCheck<GameState>::legal(Move& move) const {
  if (move.drop() == Piece() &&
      move.index() != -1 &&
      move.pool() != -1) {
    move.setDrop(m_state.pools().pool(move.pool()).get(move.index()));
  }
  
  return true;
}

template <typename GameState>
InteractionType LegalityCheck<GameState>::movable(const TurnTest& test, const Point& p) {
  Piece piece = m_state.board().get(p);
  return piece != Piece() && test(piece.color()) ? Moving : NoAction;
}

template <typename GameState>
InteractionType LegalityCheck<GameState>::droppable(const TurnTest& test, int index) {
  return test(index) ? Moving : NoAction;
}


} // namespace Dummy
} // namespace HLVariant


#endif // HLVARIANT__DUMMY__LEGALITYCHECK_H
