/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "animator.h"

template <typename Variant>
AnimationGroupPtr SimpleAnimator<Variant>::warp(const Position& final) {
  const ChessPosition* current = m_cinterface->position();
  AnimationFactory res(m_cinterface->inner());
  
  for (Point i = current->first(); i <= current->last(); i = current->next(i)) {
    ChessPiece c = current->get(i);
    ChessPiece f = final.get(i);
  
    if( !c && f ) {
      //current->set(i, f);
      NamedSprite sprite = m_cinterface->setPiece(i, f, false);
      res.addPreAnimation(Animate::appear(sprite), Animate::Instant);
    }
    else if (c && !f) {
      NamedSprite old_sprite = m_cinterface->takeSprite(i);
      res.addPreAnimation(Animate::disappear(old_sprite), Animate::Instant);
    }
    else if(c && f && !(c == f) ) {
      NamedSprite old_sprite = m_cinterface->takeSprite(i);
      NamedSprite sprite = m_cinterface->setPiece(i, f, false);
      res.addPreAnimation(Animate::morph(old_sprite, sprite), Animate::Instant);
    }
  }
  
  return res;
}


