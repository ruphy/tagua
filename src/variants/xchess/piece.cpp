/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "piece.h"

ChessPiece::ChessPiece(ChessPiece::Color color, ChessPiece::Type type)
: m_color(color)
, m_type(type) { }

ChessPiece::ChessPiece(const ChessPiece& other)
: m_color(other.color())
, m_type(other.type()) { }

ChessPiece& ChessPiece::operator=(const ChessPiece& other) {
  m_color = other.m_color;
  m_type = other.m_type;
  return *this;
}

ChessPiece* ChessPiece::clone() const {
  return new ChessPiece(*this);
}

bool ChessPiece::equals(const ChessPiece* other) const {
  if (other) {
    if (!valid())
      return false;
    else
      return m_color == other->color() && m_type == other->type();
  }
  else
    return !valid();
}

bool ChessPiece::operator==(const ChessPiece& other) const {
  if (other.valid())
    return equals(&other);
  else
    return equals(0);
}

bool ChessPiece::sameColor(const ChessPiece* other) const {
  return other && (m_color == other->color());
}

int ChessPiece::id() const {
  int res = static_cast<int>(type());
  if (color() == BLACK) res = -res - 1;
  return res;
}

ChessPiece::Color ChessPiece::colorFromId(int x) {
  return x < 0 ? BLACK : WHITE;
}

ChessPiece::Type ChessPiece::typeFromId(int x) {
  return static_cast<Type>(x<0 ? (-1-x) : x);
}

QString ChessPiece::typeName() const {
  switch (m_type) {
  case ROOK:
    return "rook";
  case BISHOP:
    return "bishop";
  case KNIGHT:
    return "knight";
  case QUEEN:
    return "queen";
  case KING:
    return "king";
  case PAWN:
    return "pawn";
  default:
    return "unknown";
  }
}

QString ChessPiece::name() const {
  switch(m_color) {
    case WHITE:
      switch (m_type) {
      case ROOK:
        return "white_rook";
      case BISHOP:
        return "white_bishop";
      case KNIGHT:
        return "white_knight";
      case QUEEN:
        return "white_queen";
      case KING:
        return "white_king";
      case PAWN:
        return "white_pawn";
      default:
        return "unknown";
      }
    case BLACK:
      switch (m_type) {
      case ROOK:
        return "black_rook";
      case BISHOP:
        return "black_bishop";
      case KNIGHT:
        return "black_knight";
      case QUEEN:
        return "black_queen";
      case KING:
        return "black_king";
      case PAWN:
        return "black_pawn";
      default:
        return "unknown";
      }
    default:
      return "unknown";
  }
}

ChessPiece::Type ChessPiece::getType(const QString& str) {
  // assume the string is well formatted
  if (str.isEmpty())
    return PAWN;

  char c = str[0].toAscii();
  switch (c) {
  case 'Q':
  case 'q':
    return QUEEN;
  case 'N':
  case 'n':
    return KNIGHT;
  case 'B':
  case 'b':
    return BISHOP;
  case 'K':
  case 'k':
    return KING;
  case 'R':
  case 'r':
    return ROOK;
  case 'P':
  case 'p':
    return PAWN;
  default:
    return INVALID_TYPE;
  }
}

QString ChessPiece::typeSymbol(ChessPiece::Type type) {
  switch (type) {
  case QUEEN:
    return "Q";
  case KING:
    return "K";
  case KNIGHT:
    return "N";
  case BISHOP:
    return "B";
  case ROOK:
    return "R";
  case PAWN:
    return "P";
  default:
    return "?";
  }
}

std::ostream& operator<<(std::ostream& os, const ChessPiece& p) {
  return os << (p.color() == WHITE? "white" : p.color() == BLACK? "black" : "unknown")
                                                        << " " << p.typeName();
}

