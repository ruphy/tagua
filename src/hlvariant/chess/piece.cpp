#include "piece.h"

namespace HLVariant {
namespace Chess {

Piece::~Piece() { }

Piece::Piece(Color color, Type type)
: m_color(color)
, m_type(type) { }

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

QString Piece::name() const {
  return colorName() + '_' + typeName();
}

Piece::Color Piece::oppositeColor(Color color) {
  switch (color) {
  case WHITE:
    return BLACK;
  case BLACK:
    return WHITE;
  default:
    return INVALID_COLOR;
  }
}

bool Piece::operator==(const Piece& other) const {
  return m_color == other.m_color &&
         m_type == other.m_type;
}

bool Piece::operator!=(const Piece& other) const {
  return !((*this) == other);
}

} // namespace HLVariant
} // namespace Chess

