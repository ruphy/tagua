/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANTS__VARIANTDATA_H
#define HLVARIANTS__VARIANTDATA_H

template <typename Variant>
struct VariantData {
  typedef typename Variant::MoveGenerator MoveGenerator;
  typedef typename MoveGenerator::LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename GameState::Pool Pool;
  typedef typename GameState::Move Move;
  typedef typename Board::Piece Piece;
  typedef typename Variant::Animator Animator;  
  typedef typename Variant::Serializer Serializer;
  typedef typename Variant::MoveFactory MoveFactory;
};

#endif // HLVARIANTS__VARIANTDATA_H

