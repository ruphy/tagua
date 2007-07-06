/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <math.h>
#include <iostream>
#include <functional>
#include <algorithm>
#include <boost/scoped_ptr.hpp>

#include "common.h"
#include "animation.h"
#include "point.h"
#include "sprite.h"
#include "movement.h"

using namespace std;
using namespace boost;

#undef ANIMATION_DEBUG

//BEGIN ConcreteAnimation

ConcreteAnimation::ConcreteAnimation(const SpritePtr& piece)
: m_piece(piece) { }

//END ConcreteAnimation

//BEGIN MovementAnimation

MovementAnimation::MovementAnimation(const SpritePtr& piece,
                            const QPoint& destination, double speed)
: ConcreteAnimation(piece)
, m_source(piece)
, m_target(piece)
, m_destination(destination)
, m_speed(speed)
, m_state(Inactive) {
#ifdef ANIMATION_DEBUG
  cout << "creating animation " << this << " for piece " << piece.get() << endl;
#endif
}

MovementAnimation::~MovementAnimation() {
#ifdef ANIMATION_DEBUG
  cout << "destroying animation " << this << " for piece " << m_piece.get() << endl;
  if (m_piece->movementAnimation().lock())
    cout << " ***********    BUG    **************" << endl;
#endif
//  m_piece->setMovementAnimation(0);
}

void MovementAnimation::setTarget(const SpritePtr& target) {
  m_target = target;
}

void MovementAnimation::setSource(const SpritePtr& source) {
  m_source = source;
}

boost::shared_ptr<Movement>
MovementAnimation::createMovement(const QPoint& from, const QPoint& to) const {
  return boost::shared_ptr<Movement>(new SigmoidalMovement<LinearMovement>(from, to));
}

void MovementAnimation::start() {
#ifdef ANIMATION_DEBUG
  cout << "starting movement animation " << this
       << " on piece " << m_piece.get() << endl;
#endif

  {
    if (shared_ptr<Animation> animation = m_source->movementAnimation().lock()) {
#ifdef ANIMATION_DEBUG
      cout << "aborting movement animation " << animation
           << " on piece " << m_source.get() << endl;
#endif
      animation->abort();
    }
  }

  m_source->setMovementAnimation(shared_ptr<Animation>());
  m_target->setMovementAnimation(shared_from_this());

  m_piece->raise();
  m_movement = createMovement(m_piece->pos(), m_destination);

  // The total time of the animation is proportional to the square root
  // of the distance and to the inverse of the speed factor.
  double distance = Point(m_destination - m_piece->pos()).norm();

  Q_ASSERT(m_speed > 0.0);
  m_time = 35 * sqrt(distance) / m_speed;
  m_state = m_time > 0 ? Active : Inactive;
}

Animation::State MovementAnimation::animationAdvance(int msec) {
  switch (m_state) {
  case Inactive:
    m_start = msec;
    start();
    if (m_state == Inactive) abort();
    break;
  case Aborted:
    break;
  case Active:
    {
      // reparameterize the time variable in [0,1]
      Q_ASSERT(m_time > 0);
      double t = (msec - m_start) / m_time;
      Q_ASSERT(t >= 0);

#ifdef ANIMATION_DEBUG
      cout << "active: t = " << t << endl;
#endif

      if (t >= 1)
        stop();
      else {
        QPoint to = m_movement->pos(t);
        m_piece->moveTo(to);
        m_piece->setRotation(m_movement->rotation(t));
      }
    }

    break;
  }

#ifdef ANIMATION_DEBUG
  cout << "advance (" << this << ") state = " << m_state << endl;
#endif
  return m_state;
}

void MovementAnimation::stop() {
#ifdef ANIMATION_DEBUG
  cout << "stopping animation " << this
       << " on piece " << m_piece.get() << endl;
#endif
  m_piece->moveTo(m_destination);
  m_piece->setRotation(0);
  abort();
  m_state = Inactive;
}

void MovementAnimation::abort() {
#ifdef ANIMATION_DEBUG
  cout << "aborting animation " << this
       << " on piece " << m_piece.get() << endl;
#endif
  m_state = Aborted;
  m_target->setMovementAnimation(shared_ptr<MovementAnimation>());
}

//END MovementAnimation

//BEGIN KnightMovementAnimation

KnightMovementAnimation::KnightMovementAnimation(const SpritePtr& piece, const QPoint& destination,
                                                                       bool rotate, double speed)
: MovementAnimation(piece, destination, speed)
, m_rotate(rotate) {
}

boost::shared_ptr<Movement>
KnightMovementAnimation::createMovement(const QPoint& from, const QPoint& to) const {
  return boost::shared_ptr<Movement>(new SigmoidalMovement<LMovement>(from, to, m_rotate));
}

//END KnightMovementAnimation

//BEGIN OneShotAnimation

OneShotAnimation::OneShotAnimation(const SpritePtr& piece)
: ConcreteAnimation(piece) { }

Animation::State OneShotAnimation::animationAdvance(int) {
  shoot();
  return Inactive;
}

//END OneShotAnimation

//BEGIN InstantAnimation

InstantAnimation::InstantAnimation(const SpritePtr& piece, const QPoint& destination)
: OneShotAnimation(piece)
, m_destination(destination) { }

void InstantAnimation::shoot() {
  m_piece->moveTo(m_destination);

  if (shared_ptr<Animation> animation = m_piece->movementAnimation().lock()) {
    animation->abort();
    m_piece->setMovementAnimation(shared_ptr<Animation>());
  }
}

//END InstantAnimation

//BEGIN CaptureAnimation

CaptureAnimation::CaptureAnimation(const SpritePtr& piece)
: OneShotAnimation(piece) { }

void CaptureAnimation::shoot() {
  m_piece->hide();

  if (shared_ptr<Animation> animation = m_piece->movementAnimation().lock()) {
    animation->abort();
    m_piece->setMovementAnimation(shared_ptr<Animation>());
  }

}

//END CaptureAnimation

//BEGIN DropAnimation

DropAnimation::DropAnimation(const SpritePtr& piece)
: OneShotAnimation(piece)
, m_valid_position(false) { }

DropAnimation::DropAnimation(const SpritePtr& piece, const QPoint& pos)
: OneShotAnimation(piece)
, m_valid_position(true)
, m_position(pos) { }

void DropAnimation::shoot() {
  if (m_valid_position)
    m_piece->moveTo(m_position);
  m_piece->show();
}

//END DropAnimation

//BEGIN PromotionAnimation

PromotionAnimation::PromotionAnimation(const SpritePtr& piece,
                                       const SpritePtr& promoted)
: OneShotAnimation(piece)
, m_promoted(promoted) { }

void PromotionAnimation::shoot() {
  m_piece->hide();
  m_promoted->show();
}

//END PromotionAnimation

//BEGIN CrossFadingAnimation

CrossFadingAnimation::CrossFadingAnimation(const SpritePtr& piece,
                                           const SpritePtr& promoted)
: m_piece(piece) {
  addPreAnimation(shared_ptr<FadeAnimation>(new FadeAnimation(piece, piece->pos(), 255, 0)));
  addPreAnimation(shared_ptr<FadeAnimation>(new FadeAnimation(promoted, promoted->pos(), 0, 255)));
}

void CrossFadingAnimation::start() {
  AnimationGroup::start();
}

//END CrossFadingAnimation



//BEGIN DelayAnimation

DelayAnimation::DelayAnimation(int secs)
: Animation()
, m_state(Inactive)
, m_msecs(secs) { }

void DelayAnimation::start() {
  m_state = Active;
}

Animation::State DelayAnimation::animationAdvance(int msec) {
  switch (m_state) {
  case Inactive:
    m_start = msec;
    start();
    break;
  case Aborted:
    break;
  case Active:
    if(msec > m_start + m_msecs)
      stop();
    break;
  }

  return m_state;
}

void DelayAnimation::stop() {
  m_state = Inactive;
}

void DelayAnimation::abort() {
  m_state = Aborted;
}


//END DelayAnimation


//BEGIN FadeAnimation

FadeAnimation::FadeAnimation(const SpritePtr& sprite, const QPoint& to,
                             int fadeFrom, int fadeTo)
: ConcreteAnimation(sprite)
, m_fadeFrom(fadeFrom)
, m_fadeTo(fadeTo)
, m_to(to)
, m_state(Inactive) { }

void FadeAnimation::start() {
  m_state = Active;

  m_piece->moveTo(m_to);
  m_piece->setOpacity(m_fadeFrom);
  m_piece->show();
}

Animation::State FadeAnimation::animationAdvance(int msec) {
  switch (m_state) {
  case Inactive:
    m_start = msec;
    start();
    break;
  case Aborted:
    break;
  case Active:
    {
      double t = (msec - m_start) / 400.0;
      Q_ASSERT(t >= 0);

      if(t >= 1)
        stop();
      else {
        int op = static_cast<int>(m_fadeTo * t + m_fadeFrom * (1 - t));
        m_piece->setOpacity(op);
      }
    }
    break;
  }

  return m_state;
}

void FadeAnimation::stop() {
  m_piece->setOpacity(m_fadeTo);
  m_state = Inactive;
}

void FadeAnimation::abort() {
  m_state = Aborted;
}


//END FadeAnimation



//BEGIN GrowAnimation

GrowAnimation::GrowAnimation(const SpritePtr& sprite)
: ConcreteAnimation(sprite)
, m_state(Inactive) { }

void GrowAnimation::start() {
  m_state = Active;

  m_piece->show();
  m_piece->setScale(0.0);
}

Animation::State GrowAnimation::animationAdvance(int msec) {
  switch (m_state) {
  case Inactive:
    m_start = msec;
    start();
    break;
  case Aborted:
    break;
  case Active:
    {
      double t = (msec - m_start) / 700.0;
      Q_ASSERT(t >= 0);

      if(t >= 1)
        stop();
      else {
        t = sin(t*M_PI/2);
        m_piece->setScale(t);
      }
    }
    break;
  }

  return m_state;
}

void GrowAnimation::stop() {
  m_piece->setScale(1.0);
  m_state = Inactive;
}

void GrowAnimation::abort() {
  m_state = Aborted;
}


//END GrowAnimation


//BEGIN ExplodeAnimation

ExplodeAnimation::ExplodeAnimation(const SpritePtr& sprite, Random& random)
: ConcreteAnimation(sprite)
, m_state(Inactive)
, m_random(random) { }

void ExplodeAnimation::start() {
  m_state = Active;

  m_piece->show();
  m_piece->setupExplosion(m_random);
}

Animation::State ExplodeAnimation::animationAdvance(int msec) {
  switch (m_state) {
  case Inactive:
    m_start = msec;
    start();
    break;
  case Aborted:
    break;
  case Active:
    {
      double t = (msec - m_start) / 700.0;
      Q_ASSERT(t >= 0);

      if(t >= 1)
        stop();
      else {
        t = sin(t*M_PI/2);
        m_piece->setExplosionStep(sin(t*M_PI/2)*0.7);
        //m_piece->setScale(1.0+t*0.2);
        m_piece->setOpacity(int(cos(t*M_PI/2)*255));
      }
    }
    break;
  }

  return m_state;
}

void ExplodeAnimation::stop() {
  m_piece->setExplosionStep(0.7);
  //m_piece->setScale(1.2);
  m_state = Inactive;
}

void ExplodeAnimation::abort() {
  m_state = Aborted;
}


//END ExplodeAnimation

//BEGIN AnimationGroup

AnimationGroup::AnimationGroup(bool persistent)
    : Animation()
    , m_active(false)
    , m_persistent(persistent)
    , m_chain_abortions(!persistent) {
}

void AnimationGroup::start() {
  m_active = true;
}

Animation::State AnimationGroup::animationAdvance(int msec) {
  if (!m_active) {
    start();
  }


  if (!pre.empty()) {
    // advance pre-animations
    for (Iterator i = pre.begin(); i != pre.end(); ) {
      State animation_state = (*i)->animationAdvance(msec);
      switch (animation_state) {
      case Aborted:
#if 0
        if (m_chain_abortions) {
          for (Iterator j = pre.begin(); j != pre.end(); ) {
            (*j)->stop();
            j = pre.erase(j);
          }
          post.clear();
          return Aborted;
        }
#endif
      case Inactive:
        // the animation ended or has been aborted,
        // remove it from the list
        i = pre.erase(i);
        break;
      case Active:
        // handle next animation
        ++i;
        break;
      }
    }

    // if there are still pre-animations, we're done
    if (!pre.empty())
      return Active;
  }
  else {
    // no pre-animations: handle post animations
    for (Iterator i = post.begin(); i != post.end(); ) {
      State animation_state = (*i)->animationAdvance(msec);
      switch (animation_state) {
      case Aborted:
#if 0
        if (m_chain_abortions) {
          return Aborted;
        }
#endif
      case Inactive:
        // the animation ended or has been aborted,
        // remove it from the list
        i = pre.erase(i);
        break;
      case Active:
        // handle next animation
        ++i;
        break;
      }
    }
  }

  // go on animating if there are still animations left
  // or the group is persistent
  m_active = m_persistent || !post.empty();
  return m_active ? Active : Inactive;
}

void AnimationGroup::addPreAnimation(AnimationPtr anim) {
  if (!anim) return;

  if (m_active)
    // cannot dynamically add pre-animations
    // adding it as a post-animation, instead
    addPostAnimation(anim);
  else
    pre.push_back(anim);
}

void AnimationGroup::addPostAnimation(AnimationPtr anim) {
  if (!anim) return;
  post.push_back(anim);
}

void AnimationGroup::stop() {
  // stop all animations
  for (Iterator i = pre.begin(); i != pre.end(); ) {
    (*i)->stop();
    i = pre.erase(i);
  }
  for (Iterator i = post.begin(); i != post.end(); ) {
    (*i)->stop();
    i = post.erase(i);
  }

  if (!m_persistent)
    m_active = false;
}

//END AnimationGroup

//BEGIN TeleportAnimation

TeleportAnimation::TeleportAnimation(const SpritePtr& sprite,
                                     const QPoint& from, const QPoint& to)
  : AnimationGroup(1.0) {

  SpritePtr copy(sprite->duplicate());
  copy->show();

  addPreAnimation(AnimationPtr(new FadeAnimation(copy, from, 255, 0)));
  addPreAnimation(AnimationPtr(new FadeAnimation(sprite, to, 0, 255)));
}

TeleportAnimation::TeleportAnimation(const SpritePtr& sprite,
                                     const QPoint& to)
  : AnimationGroup(1.0) {

  SpritePtr copy(sprite->duplicate());
  copy->show();

  addPreAnimation(AnimationPtr(new FadeAnimation(copy, copy->pos(), 255, 0)));
  addPreAnimation(AnimationPtr(new FadeAnimation(sprite, to, 0, 255)));
}


//END TeleportAnimation

//BEGIN DelayedAnimationSet

DelayedAnimationSet::DelayedAnimationSet(Random& random)
: m_random(random)
, m_state(Inactive) { }

void DelayedAnimationSet::addAnimation(const shared_ptr<Animation>& animation) {
  if (animation && m_state == Inactive)
    m_animations.push_back(animation);
}

void DelayedAnimationSet::start() {
  const int n = m_animations.size();

  if (n == 0)
    m_state = Inactive;
  else {
    // animation time is proportional to the
    // number of animations
    m_time = 100 * n;

    // one animation starts immediately
    int immediate = m_random.rand(0, n - 1)();
    m_events.push_back(Event(immediate, m_start));

    // generate event times
    Random::IntegerGenerator event_time = m_random.rand(m_start, m_start + m_time);
    for (int i = 0; i < n; i++) {
      if (i == immediate) continue;
      m_events.push_back(Event(i, event_time()));
    }

    // sort events
    sort(m_events.begin(), m_events.end());

    // set next event
    m_next_event = m_events.begin();

    m_state = Active;
  }
}

Animation::State DelayedAnimationSet::animationAdvance(int msec) {
  switch (m_state) {
  case Inactive:
    m_start = msec;
    start();
    if (m_state == Inactive) abort();
    break;
  case Aborted:
    break;
  case Active:
    {
      // if there are still events left
      if (m_next_event != m_events.end()) {

        // if the event period is elapsed
        if (msec >= m_start + m_time) {
          // execute all remaining animations
          while (m_next_event != m_events.end())
            execute((m_next_event++)->index());
        }

        else {
          // if the event time has elapsed
          while (m_next_event != m_events.end() && msec >= m_next_event->time()) {
            // run the corresponding animation
            execute((m_next_event++)->index());
          }
        }
      }

      // step all active animations
      // continue animating until m_group is over
      m_state = m_group.animationAdvance(msec);
    }

    break;
  }

  return m_state;
}

void DelayedAnimationSet::execute(int index) {
  // start animation adding it to m_group
  m_group.addPostAnimation(m_animations[index]);

  // remove it from the animation vector
  m_animations[index] = shared_ptr<Animation>();
}

void DelayedAnimationSet::stop() {
  m_group.stop();
  m_animations.clear();
  m_state = Inactive;
}

void DelayedAnimationSet::abort() {
  m_state = Aborted;
  m_animations.clear();
}

//END DelayedAnimationSet
