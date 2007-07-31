#ifndef HLVARIANT__CHESS__VARIANT_H
#define HLVARIANT__CHESS__VARIANT_H

#include "movegenerator.h"
#include "serializer.h"

namespace HLVariant {
namespace Chess {

struct Variant {
  typedef LegalityCheck<GameState<Board<Piece>, Move> > LegalityCheck;
  typedef MoveGenerator<LegalityCheck> MoveGenerator;
  typedef Serializer<MoveGenerator> Serializer;
  
  static const char* m_name;
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__VARIANT_H
