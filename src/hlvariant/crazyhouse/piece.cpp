/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "piece.h"

namespace HLVariant {
namespace Crazyhouse {

Piece::Piece(Color color, Type type)
: Chess::Piece(color, type)
, m_promoted(false) { }

bool Piece::operator==(const Piece& other) const {
  return Chess::Piece::operator==(other) && m_promoted == other.m_promoted;
}

void Piece::setPromoted() {
  m_promoted = true;
}

Piece::Type Piece::actualType() const {
  if (m_promoted)
    return PAWN;
  else
    return type();
}

} // namespace Crazyhouse
} // namespace HLVariant

