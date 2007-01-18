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
#include "wrappedanimator.h"

class AbstractPosition;
class AbstractMove;
class AnimationGroup;
class PointConverter;
class GraphicalPosition;
class ChessMove;
class ImageLoaderInfo;
class ChessVariant;

/**
  * A variant-agnostic animator. 
  * Can be used as a base class for other specialized animators.
  */
template <typename Variant>
class SimpleAnimator {
protected:
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
  typedef GenericGraphicalPosition<Variant> GPosition;
  typedef typename WrappedGraphicalPosition<Variant>::GElement GElement;
  typedef typename Variant::Position Position;
  typedef typename Variant::Piece Piece;
  typedef typename Variant::Move Move;
  
  PointConverter* m_converter;
  boost::shared_ptr<GPosition> m_position;
  Random m_random;
  
  bool m_anim_movement;
  bool m_anim_explode;
  bool m_anim_fade;
  bool m_anim_rotate;
  
  virtual boost::shared_ptr<MovementAnimation>
    createMovementAnimation(const GElement& element, const QPoint& destination);

  virtual boost::shared_ptr<Animation> createCapture(const Point& p,
                                                     const GElement& piece,
                                                     const GElement& captured,
                                                     const Position& pos);

  virtual void finalizeBackAnimation(AnimationPtr,
                                     const Position&,
                                     const Move&) { }
  virtual void finalizeForwardAnimation(AnimationPtr,
                                        const Position&,
                                        const Move&) { }
public:
  SimpleAnimator(PointConverter* converter, const boost::shared_ptr<GPosition>& position);
  virtual ~SimpleAnimator() { }
  AnimationPtr warp(const Position&);
  AnimationPtr forward(const Position&, const Move& move);
  AnimationPtr back(const Position&, const Move& move);
};

#endif // ANIMATOR_H
