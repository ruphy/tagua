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

#include "point.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename _Move, typename _Piece>
class MoveMixin : public _Move {
public:
  typedef _Move Move;
  typedef _Piece Piece;
private:
  typedef typename Piece::Color Color;
  
  Color m_pool;
  int m_index;
  Piece m_drop;
public:
  MoveMixin();
  MoveMixin(const Point& from, const Point& to, int promotionType = -1);
  MoveMixin(Color pool, int index, const Point& to);
  MoveMixin(const Piece& dropped, const Point& to);
  
  Piece drop() const;
  void setDrop(const Piece& piece);
  
  Color pool() const;
  int index() const;
};

// IMPLEMENTATION

template <typename Move, typename Piece>
MoveMixin<Move, Piece>::MoveMixin()
: m_pool(Piece::INVALID_COLOR)
, m_index(-1) { }

template <typename Move, typename Piece>
MoveMixin<Move, Piece>::MoveMixin(const Point& from, const Point& to, int promotionType)
: Move(from, to, promotionType)
, m_pool(Piece::INVALID_COLOR)
, m_index(-1) { }

template <typename Move, typename Piece>
MoveMixin<Move, Piece>::MoveMixin(Color pool, int index, const Point& to)
: Move(Point::invalid(), to)
, m_pool(pool)
, m_index(index) { }

template <typename Move, typename Piece>
MoveMixin<Move, Piece>::MoveMixin(const Piece& dropped, const Point& to)
: Move(Point::invalid(), to)
, m_pool(dropped.color())
, m_index(-1)
, m_drop(dropped) { }

template <typename Move, typename Piece>
Piece MoveMixin<Move, Piece>::drop() const {
  return m_drop;
}

template <typename Move, typename Piece>
void MoveMixin<Move, Piece>::setDrop(const Piece& drop) {
  m_drop = drop;
}

template <typename Move, typename Piece>
typename Piece::Color MoveMixin<Move, Piece>::pool() const {
  return m_pool;
}

template <typename Move, typename Piece>
int MoveMixin<Move, Piece>::index() const {
  return m_index;
}

}; // namespace Crazyhouse
}; // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__MOVE_H

