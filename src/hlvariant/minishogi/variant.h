/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
	    (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#ifndef HLVARIANT__MINISHOGI__VARIANT_H
#define HLVARIANT__MINISHOGI__VARIANT_H

#include "../shogi/piece.h"
#include "../shogi/shogiban.h"
#include "../chess/move.h"
#include "../crazyhouse/move.h"
#include "gamestate.h"
#include "../shogi/legalitycheck.h"
#include "../shogi/serializer.h"
#include "../crazyhouse/movefactory.h"
#include "../animator.h"
#include "../dropanimator.h"

namespace HLVariant {
namespace MiniShogi {

struct TAGUA_EXPORT Variant {
  typedef Crazyhouse::MoveMixin<Chess::Move, Shogi::Piece> Move;
  typedef GameState<Shogi::ShogiBan<5, 5, Shogi::Piece>, Move> GameState;
  typedef Shogi::LegalityCheck<GameState> LegalityCheck;
  typedef Shogi::Serializer<LegalityCheck> Serializer;
  typedef DropAnimatorMixin<SimpleAnimator<Variant> > Animator;
  typedef Crazyhouse::MoveFactory<GameState> MoveFactory;
  typedef Variant MoveGenerator; // to make VariantData happy
  
  static const bool hasICS = false;
  static const bool m_simple_moves = false;
  static const char *m_name;
  static const char *m_theme_proxy;

  static int moveListLayout() { return 0; }

  static OptList positionOptions() { return OptList(); }
  
};

} // namespace MiniShogi
} // namespace HLVariant

#endif // HLVARIANT__MINISHOGI__VARIANT_H
