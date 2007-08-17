/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#ifndef HLVARIANT__DROPANIMATOR_H
#define HLVARIANT__DROPANIMATOR_H

#include "animationfactory.h"
#include "namedsprite.h"
#include "common.h"

namespace HLVariant {

template <typename Base>
class DropAnimatorMixin : public Base {
  typedef typename Base::Variant Variant;
  using Base::m_cinterface;
protected:
  typedef typename Base::API API;
  typedef typename Base::GameState GameState;
  typedef typename Base::Piece Piece;
  typedef typename Base::Move Move;

  virtual void updatePool(const GameState& final);
public:
  DropAnimatorMixin(API cinterface);

  virtual AnimationGroupPtr warp(const GameState& final);
  virtual AnimationGroupPtr forward(const GameState& final, const Move& move);
  virtual AnimationGroupPtr back(const GameState& final, const Move& move);
};

// IMPLEMENTATION

template <typename Base>
DropAnimatorMixin<Base>::DropAnimatorMixin(API cinterface)
: Base(cinterface) { }


template <typename Base>
void DropAnimatorMixin<Base>::updatePool(const GameState& final) {
  for(int color = 0; color < 2; color++) {
    typename Piece::Color c = static_cast<typename Piece::Color>(color);
    const typename GameState::Pool pool = final.pools().pool(c);
    const int n = pool.size();
    
    for (int index = 0; index < n; ) {
      // precondition: pool and graphical pool match up to index
    
      // no more sprites on the graphical pool
      if (index >= m_cinterface->poolSize(color)) {
        // add extra sprites
        for (int i = index; i < n; i++)
          m_cinterface->insertPoolPiece(color, i, pool.get(i));
        
        // done
        break;
      }
      
      NamedSprite sprite = m_cinterface->getPoolSprite(color, index);
      int i;
      
      // find a matching piece on the pool
      for (i = index; i < n; i++) {
        if (pool.get(i).name() == sprite.name())
          break;
      }
      
      if (i < n) {
        // matching piece found on the pool
        // insert all pieces before this one on the graphical pool
        for (int j = index; j < i; j++)
          m_cinterface->insertPoolPiece(color, j, pool.get(j));
        index = i + 1;
      }
      else {
        // no such piece found: remove it from the graphical pool
        m_cinterface->removePoolSprite(color, index);
      }
    }
    
    // remove extra pieces from the graphical pool
    while (m_cinterface->poolSize(color) > n)
      m_cinterface->removePoolSprite(color, n);
  }
}

template <typename Base>
AnimationGroupPtr DropAnimatorMixin<Base>::warp(const GameState& final) {
  updatePool(final);
  return Base::warp(final);
}

template <typename Base>
AnimationGroupPtr DropAnimatorMixin<Base>::forward(const GameState& final, const Move& move) {
  AnimationFactory res(m_cinterface->inner());

  if (move.drop() != Piece()) {
    NamedSprite captured = m_cinterface->takeSprite(move.to());
  
    std::pair<int, int> dropped = m_cinterface->droppedPoolPiece();
    if (dropped.first != -1 && dropped.second != -1
        && m_cinterface->position()->pools().pool(
          static_cast<typename Piece::Color>(dropped.first)
        ).get(
          static_cast<typename Piece::Type>(dropped.second)
        ) == move.drop()) {
      NamedSprite drop = m_cinterface->takePoolSprite(dropped.first, dropped.second);
      m_cinterface->setSprite(move.to(), drop);
      res.addPreAnimation(Animate::move(drop, move.to()));
    }
    else {      
      NamedSprite drop = m_cinterface->setPiece(move.to(), move.drop(), false);
      drop.sprite()->raise();
      res.addPreAnimation(Animate::appear(drop));
    }
    
      
    if (captured)
      res.addPostAnimation(Animate::destroy(captured));
    res.group()->addPostAnimation(warp(final));
  }
  else {
    res.setGroup(Base::forward(final, move));
  }
  
  return res;
}

template <typename Base>
AnimationGroupPtr DropAnimatorMixin<Base>::back(const GameState& final, const Move& move) {
  AnimationFactory res(m_cinterface->inner());

  if(move.drop() != Piece()) {
    NamedSprite drop = m_cinterface->takeSprite(move.to());
    res.addPostAnimation(Animate::disappear(drop));
    
    res.group()->addPostAnimation(warp(final));
  }
  else {
    res.setGroup(Base::back(final, move));
  }

  return res;
}

}

#endif // HLVARIANT__DROPANIMATOR_H

