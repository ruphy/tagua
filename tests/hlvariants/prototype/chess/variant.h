#ifndef HLVARIANT__CHESS__VARIANT_H
#define HLVARIANT__CHESS__VARIANT_H

#include "legalitycheck.h"

namespace HLVariant {
namespace Chess {

struct Variant {
  typedef LegalityCheck<GameState<Board<Piece>, Move> > LegalityCheck;
  
  static const char* m_name;
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__VARIANT_H
