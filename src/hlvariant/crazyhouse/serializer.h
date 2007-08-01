/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__SERIALIZER_H
#define HLVARIANT__CRAZYHOUSE__SERIALIZER_H

#include "../chess/serializer.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename MoveGenerator>
class Serializer : public Chess::Serializer<MoveGenerator> {
  typedef Chess::Serializer<MoveGenerator> Base;
  typedef typename Base::GameState GameState;
  typedef typename Base::Piece Piece;
  typedef typename Base::Move Move;
protected:
  using Base::m_rep;
public:
  Serializer(int rep);
  virtual QString serialize(const Move& move, const GameState& ref);
};

// IMPLEMENTATION

template <typename MoveGenerator>
Serializer<MoveGenerator>::Serializer(int rep)
: Base(rep) { }

template <typename MoveGenerator>
QString Serializer<MoveGenerator>::serialize(const Move& move, const GameState& ref) {
  if (move.drop() == Piece()) {
    return Base::serialize(move, ref);
  }
  
  QString res;
  switch (m_rep) {
  case Base::SIMPLE:
  case Base::COMPACT:
    res = symbol(move.drop().type()).toUpper();
  case Base::DECORATED:
    res = "{" + move.drop().typeName() + '}';
  }
  
  return res + '@' + move.to().toString(ref.board().size().y);
}


} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__SERIALIZER_H

