/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__MOVE_H
#define HLVARIANT__CRAZYHOUSE__MOVE_H

namespace HLVariant {
namespace Crazyhouse {

template <typename Piece>
class Move : public Chess::Move {
public:
  Move();
  Move(const Point& from, const Point& to, int promotionType = -1);
  
  Piece drop() const;
};

template <typename Piece>
Move<Piece>::Move() { }

template <typename Piece>
Move<Piece>::Move(const Point& from, const Point& to, int promotionType)
: Chess::Move(from, to, promotionType) { }

template <typename Piece>
Piece Move<Piece>::drop() const {
  return Piece(); // BROKEN
}

}; // namespace Crazyhouse
}; // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__MOVE_H

