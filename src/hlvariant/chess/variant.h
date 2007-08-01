#ifndef HLVARIANT__CHESS__VARIANT_H
#define HLVARIANT__CHESS__VARIANT_H

#include "movegenerator.h"
#include "serializer.h"
#include "export.h"
#include "option.h"
#include "../animator.h"

class VariantInfo;

namespace HLVariant {

namespace Chess {

struct TAGUA_EXPORT Variant {
  typedef LegalityCheck<GameState<Board<Piece>, Move> > LegalityCheck;
  typedef MoveGenerator<LegalityCheck> MoveGenerator;
  typedef Serializer<MoveGenerator> Serializer;
  typedef SimpleAnimator<Variant> Animator;
  
  static const bool hasICS = false;
  static const bool m_simple_moves = false;
  static const char *m_name;
  static const char *m_theme_proxy;

  static int moveListLayout() { return 0; }

  static VariantInfo* static_chess_variant;
  static VariantInfo* info();
  static OptList positionOptions() { return OptList(); }
  
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__VARIANT_H
