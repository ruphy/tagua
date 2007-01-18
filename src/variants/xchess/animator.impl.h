/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <memory>

#include "variants/xchess/animator.h"
#include "variants/xchess/move.h"
#include "animation.h"
#include "graphicalposition.h"
#include "piecesprite.h"
#include "kboard.h"
#include "pointconverter.h"
#include "position.h"

using namespace boost;

template <typename Variant>
SimpleAnimator<Variant>::SimpleAnimator(PointConverter* converter,
                                        const shared_ptr<GPosition>& position)
: m_converter(converter)
, m_position(position) {
  if(position->getBoolSetting("animations", true)) {
    m_anim_movement = (bool)position->getBoolSetting("animations.movement", true);
    m_anim_explode = (bool)position->getBoolSetting("animations.explode", true);
    m_anim_fade = (bool)position->getBoolSetting("animations.fading", true);
    m_anim_rotate = (bool)position->getBoolSetting("animations.transform", true);
  }
}

template <typename Variant>
shared_ptr<AnimationGroup> SimpleAnimator<Variant>::warp(const Position& final) {
  AnimationPtr res(new AnimationGroup);
  // for each point
  for (Point i = m_position->first(); i <= m_position->last(); i = m_position->next(i)) {
    QPoint real = m_converter->toReal(i);
    
    // retrieve graphical element
    GElement p = m_position->getElement(i);
    
    // retrieve piece
    const Piece* q = final.get(i);
    
    // create animation
    shared_ptr<Animation> a;

    // if the graphical element is valid
    if (p) {
      // assert that its sprite is valid as well
      shared_ptr<PieceSprite> sprite = p.sprite();
      Q_ASSERT(sprite);

      // if the piece differs
      if (!p.piece().equals(q)) {
        shared_ptr<PieceSprite> sprite = p.sprite();

        if (q) {
          // replace sprite        
          a = shared_ptr<Animation>(new PromotionAnimation( sprite,
                                          m_position->setPiece(i, *q, false, false) ));
        }
        else {
          // remove it
          m_position->removeElement(i);
          a = shared_ptr<Animation>(new CaptureAnimation(sprite));
        }
      }
    }
    // if there is no graphical element, simply add it
    else if (q) {
      a = shared_ptr<Animation>(new DropAnimation( m_position->setPiece(i, *q, false, false) ));
    }

    // add animation to the group
    if (a) res->addPreAnimation(a);
  }

  return res;
}

template <typename Variant>
shared_ptr<MovementAnimation> SimpleAnimator<Variant>::createMovementAnimation(
                                const GElement& element,
                                const QPoint& destination) {
  return shared_ptr<MovementAnimation>(new MovementAnimation(element.sprite(),
                                                             destination, 1.0));
}


template <typename Variant>
shared_ptr<Animation> SimpleAnimator<Variant>::createCapture(const Point& p,
                                                         const GElement& /*piece*/,
                                                         const GElement& captured,
                                                         const Position& /*pos*/) {
  if (!captured)
    return shared_ptr<Animation>();
  else if(m_anim_explode)
    return shared_ptr<Animation>(new ExplodeAnimation(captured.sprite(), m_random));
  else if(m_anim_fade)
    return shared_ptr<Animation>(new FadeAnimation(captured.sprite(),
                                              m_converter->toReal(p), 255, 0));
  else
    return shared_ptr<Animation>(new CaptureAnimation(captured.sprite()));
}

template <typename Variant>
shared_ptr<AnimationGroup> SimpleAnimator<Variant>::forward(const Position& final, 
                                                            const Move& move) {
  Q_ASSERT(m_position->consistent());

  AnimationPtr res(new AnimationGroup);

  GElement piece = m_position->getElement(move.from);
  
  if (piece) {
    m_position->removeElement(move.from);
    Q_ASSERT(m_position->consistent());
    
    if (move.to.valid()) {
      GElement captured = m_position->getElement(move.to);
    
      m_position->setElement(move.to, piece);
      Q_ASSERT(m_position->consistent());
      
      shared_ptr<Animation> mainAnimation;
      QPoint hotSpot = piece.sprite()->pos();
      hotSpot += QPoint(piece.sprite()->pixmap().width() / 2, piece.sprite()->pixmap().height() / 2);
    
    	// do not animate forward
      if (!m_anim_movement) {
        mainAnimation = shared_ptr<InstantAnimation>(
          new InstantAnimation(piece.sprite(), m_converter->toReal(move.to))
        );
      }
      // the piece is already on the destination square:
      // do a direct animation
      else if (m_converter->toLogical(hotSpot) == move.to) {
        mainAnimation = shared_ptr<MovementAnimation>(
          new MovementAnimation(piece.sprite(), m_converter->toReal(move.to))
        );
      }
      else
      // do an animation according to preferences
        mainAnimation = createMovementAnimation(piece, m_converter->toReal(move.to));
        
      res->addPreAnimation(mainAnimation);
      res->addPostAnimation(createCapture(move.to, piece, captured, final));
    }
  }
  // if it is a one-click move, do nothing
  // the warping animation will adjust everything

  // add main animation
  // the animation must be direct when the piece is already
  // near the destination square

  Q_ASSERT(m_position->consistent());

  finalizeForwardAnimation(res, final, move);

  AnimationPtr warpingAnimation(new AnimationGroup);
  warpingAnimation->addPreAnimation(res);
  warpingAnimation->addPostAnimation(warp(final));

  return warpingAnimation;
}


template <typename Variant>
shared_ptr<AnimationGroup> SimpleAnimator<Variant>::back(const Position& final,
                                                         const Move& move) {
  AnimationPtr res(new AnimationGroup);
  const Piece* captured = final.get(move.to);
  GElement piece = m_position->getElement(move.to);

  // the piece could have disappeared
  // make it reappear!
  if (!piece) {
    Q_ASSERT(final.get(move.from));
    const Piece* disappeared = final.get(move.from);
    shared_ptr<PieceSprite> sprite = m_position->setPiece(move.to, *disappeared, false, false);
    res->addPreAnimation(shared_ptr<Animation>(new DropAnimation(sprite)));
    piece = m_position->getElement(move.to);
  }

  m_position->setElement(move.from, piece);

  if (captured) {
    // recreate captured piece sprite
    QPoint real = m_converter->toReal(move.to);
    if(m_anim_fade)
      res->addPreAnimation( shared_ptr<Animation>(new FadeAnimation(
              m_position->setPiece(move.to, *captured, false, false), real, 0, 255 ) ));
    else
      res->addPreAnimation( shared_ptr<Animation>(new DropAnimation(
              m_position->setPiece(move.to, *captured, false, false) ) ));
  }
  else
    m_position->removeElement(move.to);

  res->addPreAnimation(
    m_anim_movement
    ? shared_ptr<Animation>(createMovementAnimation(piece, m_converter->toReal(move.from)))
    : shared_ptr<Animation>(new InstantAnimation(piece.sprite(), m_converter->toReal(move.from)))
  );

  finalizeBackAnimation(res, final, move);

  AnimationPtr warpingAnimation(new AnimationGroup);
  warpingAnimation->addPreAnimation(res);
  warpingAnimation->addPostAnimation(warp(final));

  return warpingAnimation;
}




