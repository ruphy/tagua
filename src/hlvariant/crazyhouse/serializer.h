/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
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
  Serializer(const QString& rep);
  virtual QString serialize(const Move& move, const GameState& ref);
};

// IMPLEMENTATION

template <typename MoveGenerator>
Serializer<MoveGenerator>::Serializer(const QString& rep)
: Base(rep) { }

template <typename MoveGenerator>
QString Serializer<MoveGenerator>::serialize(const Move& move, const GameState& ref) {
  if (move.drop() == Piece()) {
    return Base::serialize(move, ref);
  }
  
  QString res;
  if (m_rep == "decorated") {
    res = "{" + move.drop().typeName() + '}';
  }
  else {
    res = symbol(move.drop().type()).toUpper();
  }
  
  return res + '@' + move.to().toString(ref.board().size().y) + suffix(move, ref);
}


} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__SERIALIZER_H

