/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__MOVEFACTORY_H
#define HLVARIANT__MOVEFACTORY_H

#include "tagua.h"
#include "usermove.h"
#include "variantdata.h"

namespace HLVariant {

template <typename GameState>
class MoveFactory {
  typedef typename GameState::Move Move;
  typedef typename GameState::Board::Piece Piece;
public:
  virtual ~MoveFactory() { }
  
  virtual Move createNormalMove(const NormalUserMove& move) {
    return Move(move.from, move.to, static_cast<typename Piece::Type>(move.promotionType()));
  }

  virtual Move createDropMove(const DropUserMove& move) {
    return Move();
  }

  virtual NormalUserMove toNormal(const Move& move) {
    return NormalUserMove(move.from, move.to);
  }
};

}

#endif // HLVARIANT__MOVEFACTORY_H

