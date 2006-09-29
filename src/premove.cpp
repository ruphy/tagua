/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "premove.h"
#include "usermove.h"

Premove::Premove(AbstractMove::Ptr move)
: m_move(move) { }

AbstractMove::Ptr Premove::execute(AbstractPosition::Ptr position) {
  if (position->testMove(m_move)) {
    return m_move;
  }
  else return AbstractMove::Ptr();
}

NormalUserMove Premove::toUserMove() const {
  return m_move->toUserMove();
}

