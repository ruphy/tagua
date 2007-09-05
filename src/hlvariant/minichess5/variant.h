/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__MINICHESS5__VARIANT_H
#define HLVARIANT__MINICHESS5__VARIANT_H

#include "../chess/variant.h"
#include "../chess/actions.h"

namespace HLVariant {
namespace Minichess5 {

struct TAGUA_EXPORT Variant {
  typedef Chess::Move Move;
  typedef Chess::Piece Piece;
  typedef CustomBoard<5, 5, Piece> Board;
  typedef Chess::GameState<Board, Move> GameState;
  typedef Chess::LegalityCheck<GameState> LegalityCheck;
  typedef Chess::MoveGenerator<LegalityCheck> MoveGenerator;
  typedef Chess::Serializer<MoveGenerator> Serializer;
  typedef SimpleAnimator<Variant> Animator;
  typedef MoveFactory<GameState> MoveFactory;
  
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
  Chess::Actions m_actions;
};

} // namespace Minichess5
} // namespace HLVariant


#endif // HLVARIANT__MINICHESS5__VARIANT_H
