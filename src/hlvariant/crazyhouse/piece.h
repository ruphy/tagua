#ifndef HLVARIANT__CRAZYHOUSE__PIECE_H
#define HLVARIANT__CRAZYHOUSE__PIECE_H

#include "../chess/piece.h"

namespace HLVariant {
namespace Crazyhouse {

class Piece : public Chess::Piece {
  bool m_promoted;
public:
  Piece(Color color = INVALID_COLOR, Type type = INVALID_TYPE);
  
  virtual bool operator==(const Piece& other) const;
  virtual void setPromoted();
  virtual Type actualType() const;
};

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__PIECE_H

