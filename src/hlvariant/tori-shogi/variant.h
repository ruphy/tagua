/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__TORI_SHOGI__VARIANT_H
#define HLVARIANT__TORI_SHOGI__VARIANT_H

#include "piece.h"
#include "../shogi/shogiban.h"
#include "../chess/move.h"
#include "../crazyhouse/move.h"
#include "gamestate.h"
#include "legalitycheck.h"
#include "serializer.h"
#include "../crazyhouse/movefactory.h"
#include "../animator.h"
#include "../dropanimator.h"
#include "../shogi/shogiactions.h"

namespace HLVariant {
namespace ToriShogi {

struct TAGUA_EXPORT Variant {
  typedef Crazyhouse::MoveMixin<Chess::Move, Piece> Move;
  typedef GameState<Shogi::ShogiBan<7, 7, Piece>, Move> GameState;
  typedef LegalityCheck<GameState> LegalityCheck;
  typedef Serializer<LegalityCheck> Serializer;
  typedef DropAnimatorMixin<SimpleAnimator<Variant> > Animator;
  typedef Crazyhouse::MoveFactory<GameState> MoveFactory;
  typedef Variant MoveGenerator; // to make VariantData happy

  static const bool hasICS = false;
  static const bool m_simple_moves = false;
  static const bool m_hidden = false;
  static const char *m_name;
  static const char *m_theme_proxy;

  static int moveListLayout() { return 0; }

  OptList positionOptions() const { return OptList(); }
  void setupMove(NormalUserMove& m) const;
  ActionCollection* actions();
private:
  Shogi::ShogiActions m_actions;
};

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__TORI_SHOGI__VARIANT_H
