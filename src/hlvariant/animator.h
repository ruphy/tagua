/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__ANIMATOR_H
#define HLVARIANT__ANIMATOR_H

#include "animation.h"
#include "fwd.h"
#include "variantdata.h"

class NamedSprite;
class Point;
class AnimationFactory;

namespace HLVariant {

/**
  * A generic animator. 
  * Can be used as a base class for other specialized animators.
  */
template <typename _Variant>
class BaseAnimator {
public:
  typedef _Variant Variant;
protected:
  typedef typename UnwrappedGraphicalAPIPtr<Variant>::type API;
  typedef typename VariantData<Variant>::GameState GameState;
  typedef typename VariantData<Variant>::Move Move;
  typedef typename VariantData<Variant>::Piece Piece;
  API m_cinterface;
public:
  BaseAnimator(API cinterface)
    : m_cinterface(cinterface) { }
  virtual ~BaseAnimator() { }

  virtual AnimationGroupPtr warp(const GameState& final);
  virtual AnimationGroupPtr forward(const GameState& final, const Move& move);
  virtual AnimationGroupPtr back(const GameState& final, const Move& move);
};
  
  
template <typename _Variant>
class SimpleAnimator : BaseAnimator<_Variant> {
public:
  typedef _Variant Variant;
private:
  typedef BaseAnimator<Variant> Base;
protected:
  typedef typename Base::API API;
  typedef typename Base::GameState GameState;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
  
  using Base::m_cinterface;
  
  virtual SchemePtr movement(const NamedSprite& sprite, const Point& from, const Point& to);
public:
  SimpleAnimator(API cinterface)
  : Base(cinterface) { }

  virtual AnimationGroupPtr warp(const GameState& final);
  virtual AnimationGroupPtr forward(const GameState& final, const Move& move);
  virtual AnimationGroupPtr back(const GameState& final, const Move& move);
};

}

#endif // HLVARIANT__ANIMATOR_H
