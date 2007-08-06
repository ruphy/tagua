/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
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
private:
  typedef typename GameState::Board Board;
  typedef typename GameState::Move Move;
  typedef typename Board::Piece Piece;
public:
  LegalityCheck(const GameState&) { }
  
  virtual bool legal(Move& move) const { return true; }
  virtual InteractionType movable(const TurnTest& test, const Point& p) { return Moving; }
  virtual InteractionType droppable(const TurnTest& test, int index) { return Moving; }
};

} // namespace Dummy
} // namespace HLVariant


#endif // HLVARIANT__DUMMY__LEGALITYCHECK_H
