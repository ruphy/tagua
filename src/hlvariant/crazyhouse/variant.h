/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#ifndef HLVARIANT__CRAZYHOUSE__VARIANT_H
#define HLVARIANT__CRAZYHOUSE__VARIANT_H

#include "movegenerator.h"
#include "serializer.h"
#include "piece.h"
#include "gamestate.h"
#include "move.h"
#include "../chess/move.h"
#include "export.h"
#include "option.h"
#include "legalitycheck.h"
#include "../animator.h"
#include "../dropanimator.h"
#include "movefactory.h"

namespace HLVariant {
namespace Crazyhouse {

struct TAGUA_EXPORT Variant {
  typedef MoveMixin<Chess::Move, Piece> Move;
  typedef CustomBoard<8, 8, Piece> Board;
  typedef GameState<Board, Move> GameState;
  typedef LegalityCheck<GameState> LegalityCheck;
  typedef MoveGenerator<LegalityCheck> MoveGenerator;
  typedef Serializer<MoveGenerator> Serializer;
  typedef DropAnimatorMixin<SimpleAnimator<Variant> > Animator;
  typedef MoveFactory<GameState> MoveFactory;
  
  static const bool hasICS = true;
  static const bool m_simple_moves = false;
  static const char *m_name;
  static const char *m_theme_proxy;

  static int moveListLayout() { return 0; }

  static OptList positionOptions() { return OptList(); }
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__VARIANT_H
