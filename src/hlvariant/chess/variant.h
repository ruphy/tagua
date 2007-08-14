/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CHESS__VARIANT_H
#define HLVARIANT__CHESS__VARIANT_H

#include <kactioncollection.h>

#include "movegenerator.h"
#include "serializer.h"
#include "export.h"
#include "option.h"
#include "../animator.h"
#include "../movefactory.h"
#include "actions.h"

class ActionCollection;

namespace HLVariant {
namespace Chess {

struct TAGUA_EXPORT Variant {
  typedef GameState<CustomBoard<8, 8, Piece>, Move> GameState;
  typedef LegalityCheck<GameState> LegalityCheck;
  typedef MoveGenerator<LegalityCheck> MoveGenerator;
  typedef Serializer<MoveGenerator> Serializer;
  typedef SimpleAnimator<Variant> Animator;
  typedef MoveFactory<GameState> MoveFactory;
  
  static const bool hasICS = true;
  static const bool m_simple_moves = false;
  static const char *m_name;
  static const char *m_theme_proxy;

  static int moveListLayout() { return 0; }
  
  OptList positionOptions() const { return OptList(); }
  void setupMove(NormalUserMove& m) const;
  
  ActionCollection* actions();
private:
  Actions m_actions;
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__VARIANT_H
