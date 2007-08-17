/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__MOVEFACTORY_H
#define HLVARIANT__CRAZYHOUSE__MOVEFACTORY_H

#include "../movefactory.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename GameState>
class MoveFactory : public HLVariant::MoveFactory<GameState> {
public:
  typedef typename HLVariant::MoveFactory<GameState> Base;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
  
  virtual Move createDropMove(const DropUserMove& drop) {
    return Move(static_cast<typename Piece::Color>(drop.pool), 
                drop.piece_index, 
                drop.to);
  }
};

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__MOVEFACTORY_H

