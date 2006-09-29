/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <boost/shared_ptr.hpp>
#include "animation.h"
#include "element.h"
#include "random.h"

class AbstractPosition;
class AbstractMove;
class AnimationGroup;
class PointConverter;
class GraphicalPosition;
class ChessMove;
class ImageLoaderInfo;

class ChessAnimator {
protected:
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
  PointConverter* m_converter;
  GraphicalPosition* m_position;
  Random m_random;

  bool m_anim_movement;
  bool m_anim_explode;
  bool m_anim_fade;
  bool m_anim_rotate;

  virtual boost::shared_ptr<MovementAnimation>
    createMovementAnimation(const Element& element, const QPoint& destination);

  virtual boost::shared_ptr<Animation> createCapture(const Point& p,
                                                     const Element& piece,
                                                     const Element& captured,
                                                     AbstractPosition::Ptr pos);

  virtual void finalizeBackAnimation(AnimationPtr,
                                     AbstractPosition::Ptr,
                                     const ChessMove&) { }
  virtual void finalizeForwardAnimation(AnimationPtr,
                                        AbstractPosition::Ptr,
                                        const ChessMove&) { }
public:
  ChessAnimator(PointConverter* converter, GraphicalPosition* position);
  virtual ~ChessAnimator(){}
  virtual AnimationPtr warp(AbstractPosition::Ptr);
  virtual AnimationPtr forward(AbstractPosition::Ptr, const ChessMove& move);
  virtual AnimationPtr back(AbstractPosition::Ptr, const ChessMove& move);
};

#endif // ANIMATOR_H
