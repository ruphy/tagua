/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOGI__SERIALIZER_H
#define HLVARIANT__SHOGI__SERIALIZER_H

#include <QString>

namespace HLVariant {
namespace Shogi {

template <typename _LegalityCheck>
class Serializer {
public:
  enum {
    SIMPLE = 0,
    COMPACT = 1,
    DECORATED = 2
  };

  typedef _LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  typedef typename GameState::Move Move;
private:
  int m_rep;
public:
  Serializer(int rep);
  
  QString serialize(const Move&, const GameState& ref);
  Move deserialize(const QString& str, const GameState& ref);
};

// IMPLEMENTATION

template <typename LegalityCheck>
Serializer<LegalityCheck>::Serializer(int rep)
: m_rep(rep) { }

template <typename LegalityCheck>
QString Serializer<LegalityCheck>::serialize(const Move&, const GameState&) {
  return "N/A"; // BROKEN
}

template <typename LegalityCheck>
typename Serializer<LegalityCheck>::Move Serializer<LegalityCheck>::deserialize(const QString&, const GameState&) {
  return Move(); // BROKEN
}

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__SERIALIZER_H

