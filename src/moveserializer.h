/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVESERIALIZER_H
#define MOVESERIALIZER_H

#include <QString>
#include "decoratedmove.h"

class AbstractMoveSerializer {
public:
  virtual ~AbstractMoveSerializer() { }
  virtual QString SAN() const = 0;
  virtual DecoratedMove toDecoratedMove() const = 0;
};

template <typename Pos>
class MoveSerializerBase : public AbstractMoveSerializer {
public:
  typedef Pos Position;
  typedef typename Position::Move Move;
  typedef typename Position::Piece Piece;
protected:
  const Move& m_move;
  const Position& m_ref;

  virtual QString checkSuffix() const {
    Pos temp(m_ref);
    temp.move(m_move);
    if (temp.check()) {
      if (temp.stalled())
        return "#";
      else
        return "+";
    }
    else
      return "";
  }

public:
  MoveSerializerBase(const Move& move, const Position& ref)
  : m_move(move)
  , m_ref(ref) { }
  virtual ~MoveSerializerBase() { }

  virtual QString SAN() const;
  virtual DecoratedMove toDecoratedMove() const;
};

template <typename Pos>
class MoveSerializer : public MoveSerializerBase<Pos> {
  typedef Pos Position;
  typedef typename Position::Move Move;
public:
  MoveSerializer(const Move& move, const Position& ref)
  : MoveSerializerBase<Position>(move, ref) { }
};

#endif // MOVESERIALIZER_H
