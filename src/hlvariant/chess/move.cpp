#include "move.h"

namespace HLVariant {
namespace Chess {

Move::Move(const Point& from, const Point& to, int promotionType)
: m_type(NORMAL)
, m_from(from)
, m_to(to)
, m_promotion(promotionType) { }

Move::Move()
: m_type(INVALID)
, m_from(Point::invalid())
, m_to(Point::invalid())
, m_promotion(-1) { }

Move::~Move() { }

Point Move::enPassantTrigger() const {
  if (m_type == EN_PASSANT_TRIGGER) {
    return (m_from + m_to) / 2;
  }
  else {
    return Point::invalid();
  }
}

Point Move::captureSquare() const {
  if (m_type == EN_PASSANT_CAPTURE) {
    return Point(m_to.x, m_from.y);
  }
  else {
    return m_to;
  }
}

int Move::promoteTo() const {
  if (m_type == PROMOTION) {
    return m_promotion;
  }
  else {
    return -1;
  }
}

bool Move::kingSideCastling() const {
  return m_type == KING_SIDE_CASTLING;
}

bool Move::queenSideCastling() const {
  return m_type == QUEEN_SIDE_CASTLING;
}

Point Move::from() const {
  return m_from;
}

Point Move::to() const {
  return m_to;
}

bool Move::valid() const {
  return //m_from != Point::invalid() &&
         m_to != Point::invalid() &&
         m_type != INVALID;
}

void Move::setType(Type type) {
  m_type = type;
}

bool Move::operator==(const Move& move) const {
  return m_type == move.m_type &&
         m_from == move.m_from &&
         m_to == move.m_to     &&
         m_promotion == move.m_promotion;
}

} // namespace Chess
} // namespace HLVariant


