/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef USERMOVE_H
#define USERMOVE_H

#include <boost/shared_ptr.hpp>
#include "fwd.h"
#include "point.h"

struct NormalUserMove {
  Point from;
  Point to;
  int promotionType;
  bool dragged;

  NormalUserMove()
  : from(Point::invalid())
  , to(Point::invalid())
  , promotionType(0) { }
  NormalUserMove(const Point& from, const Point& to, bool dragged = false)
  : from(from)
  , to(to)
  , promotionType(0)
  , dragged(dragged) {  }
  static NormalUserMove invalid() {
    return NormalUserMove();
  }

  bool valid() const {
    return from != Point::invalid() && to != Point::invalid();
  }
};

struct DropUserMove {
  PiecePtr m_piece;
  Point m_to;

  DropUserMove(PiecePtr piece, const Point& to)
  : m_piece(piece) //NOTE: clone?!?!??!?!?!
  , m_to(to) { }
};

#endif //USERMOVE_H
