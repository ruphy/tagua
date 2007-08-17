/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__DUMMY__MOVEGENERATOR_H
#define HLVARIANT__DUMMY__MOVEGENERATOR_H

namespace HLVariant {
namespace Dummy {

template <typename _LegalityCheck>
class MoveGenerator {
public:
  typedef _LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;

  MoveGenerator(const GameState& ) { }
  virtual ~MoveGenerator() { }
  
  virtual bool stalled() const { return false; }
  virtual bool check(typename Piece::Color) const { return false; }
};

} // namespace Dummy
} // namespace HLVariant


#endif // HLVARIANT__DUMMY__MOVEGENERATOR_H
