/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVEFACTORY_H
#define MOVEFACTORY_H

#include "tagua.h"
#include "usermove.h"

template <typename Variant>
struct MoveFactory {
  static typename Variant::Move createNormalMove(const NormalUserMove& move) {
    return typename Variant::Move(move.from, move.to,
            static_cast<typename Variant::Piece::PromotionType>(move.promotionType));
  }

  static typename Variant::Move createDropMove(const DropUserMove& move) {
    return Variant::Move::createDropMove(move.m_pool, move.m_piece_index, move.m_to);
  }

  static NormalUserMove toNormal(const typename Variant::Move& move) {
    return NormalUserMove(move.from, move.to);
  }
};

#endif // MOVEFACTORY_H
