#ifndef HLVARIANT__CRAZYHOUSE__VARIANT_H
#define HLVARIANT__CRAZYHOUSE__VARIANT_H

#include "../chess/movegenerator.h"
#include "../chess/serializer.h"
#include "gamestate.h"
#include "move.h"
#include "export.h"
#include "option.h"
#include "../animator.h"
#include "../dropanimator.h"

class VariantInfo;

namespace HLVariant {
namespace Crazyhouse {

struct TAGUA_EXPORT Variant {
  typedef Chess::Piece Piece;
  typedef Move<Piece> Move;
  typedef Board<Piece> Board;
  typedef GameState<Board, Move> GameState;
  typedef Chess::LegalityCheck<GameState> LegalityCheck;
  typedef Chess::MoveGenerator<LegalityCheck> MoveGenerator;
  typedef Chess::Serializer<MoveGenerator> Serializer;
  typedef DropAnimatorMixin<SimpleAnimator<Variant> > Animator;
  
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

#endif // HLVARIANT__CRAZYHOUSE__VARIANT_H
