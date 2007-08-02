/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__MOVEGENERATOR_H
#define HLVARIANT__CRAZYHOUSE__MOVEGENERATOR_H

#include "../chess/movegenerator.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename _LegalityCheck>
class MoveGenerator : public Chess::MoveGenerator<_LegalityCheck> {
  typedef Chess::MoveGenerator<_LegalityCheck> Base;
  
  using Base::m_state;
public:
  typedef typename Base::LegalityCheck LegalityCheck;
  typedef typename Base::GameState GameState;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
  typedef typename Base::MoveCallback MoveCallback;

  MoveGenerator(const GameState& m_state);
  
  virtual void generate(MoveCallback& callback) const;
};

// IMPLEMENTATION

template <typename LegalityCheck>
MoveGenerator<LegalityCheck>::MoveGenerator(const GameState& state)
: Base(state) { }

template <typename LegalityCheck>
void MoveGenerator<LegalityCheck>::generate(MoveCallback& callback) const {
  Base::generate(callback);
  
  // generate drops
  const int n = m_state.pools().pool(m_state.turn()).size();
  for (int i = 0; i < m_state.board().size().x; i++) {
    for (int j = 0; j < m_state.board().size().y; j++) {
      Point p(i, j);
      for (int k = 0; k < n; k++) {
        addMove(Move(m_state.turn(), k, p), callback);
      }
    }
  }
}

} // namespace Crazyhouse
} // namespace HLVariant


#endif // HLVARIANT__CRAZYHOUSE__MOVEGENERATOR_H
