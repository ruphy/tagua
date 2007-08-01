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
namespace Shogi {

Piece::Piece(Color color, Type type)
: m_color(color)
, m_type(type)
, m_promoted(false) { }

Piece::~Piece() { }

void Piece::setPromoted() {
  m_promoted = true;
}

Piece::Color Piece::color() const { return m_color; }

Piece::Type Piece::type() const { return m_type; }

QString Piece::colorName() const { return colorName(m_color); }

QString Piece::colorName(Color color) {
  switch (color) {
  case WHITE:
    return "white";
  case BLACK:
    return "black";
  default:
    return "unknown";
  }
}

QString Piece::typeName() const { return typeName(m_type); }

QString Piece::typeName(Type type) {
  switch (type) {
  case KING:
    return "king";
  case GOLD:
    return "gold";
  case SILVER:
    return "silver";
  case KNIGHT:
    return "knight";
  case LANCE:
    return "lance";
  case ROOK:
    return "rook";
  case BISHOP:
    return "bishop";
  case PAWN:
    return "pawn";
  default:
    return "unknown";
  }
}

QString Piece::name() const {
  QString res = colorName() + '_';
  if (m_promoted)
    res += "p_";
  return res + typeName();
}

Piece::Color Piece::oppositeColor(Color color) {
  return color == WHITE ? BLACK : WHITE;
}

bool Piece::operator==(const Piece& other) const {
  return m_color == other.m_color &&
         m_type == other.m_type &&
         m_promoted == other.m_promoted;
}

bool Piece::operator!=(const Piece& other) const {
  return !((*this) == other);
}


} // namespace Shogi
} // namespace HLVariant

