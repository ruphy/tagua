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

