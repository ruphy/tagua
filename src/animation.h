/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ANIMATION_H
#define ANIMATION_H

#include <list>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <QPoint>
#include "sprite.h"

class Movement;
class Random;

class Animation : public boost::enable_shared_from_this<Animation> {
protected:
  enum State {
    Active,
    Inactive,
    Aborted
  };
public:
  virtual ~Animation() { }
  virtual bool sticky() const { return false; }

  virtual State animationAdvance(int msec) = 0;
  virtual void stop() = 0;
  virtual void abort() = 0;
};

class ConflictingAnimation {
public:
  virtual ~ConflictingAnimation() { }
  virtual void setTarget(const SpritePtr&) = 0;
  virtual void setSource(const SpritePtr&) = 0;
};

class ConcreteAnimation : public Animation {
protected:
  SpritePtr m_piece;
public:
  ConcreteAnimation(const SpritePtr& piece);
  virtual void stop() { }
};

class OneShotAnimation : public ConcreteAnimation {
protected:
  virtual void shoot() = 0;
public:
  OneShotAnimation(const SpritePtr& piece);
  virtual State animationAdvance(int);
  virtual void abort() { }
};

class InstantAnimation : public OneShotAnimation
                       , public ConflictingAnimation {
  QPoint m_destination;
  SpritePtr m_source;
public:
  InstantAnimation(const SpritePtr& piece, const QPoint& destination);
  virtual void shoot();
  virtual void setTarget(const SpritePtr&) { }
  virtual void setSource(const SpritePtr& source) { m_source = source; }
};


class MovementAnimation : public ConcreteAnimation
                        , public ConflictingAnimation {
protected:
  SpritePtr m_source;
  SpritePtr m_target;
  QPoint m_destination;
  double m_speed;
  State m_state;

  double m_time;
  int m_start;
  boost::shared_ptr<Movement> m_movement;

  virtual boost::shared_ptr<Movement> createMovement(const QPoint& from, const QPoint& to) const;
  void start();
public:
  MovementAnimation(const SpritePtr& piece, const QPoint& destination, double speed = 1.0);
  virtual ~MovementAnimation();
  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort();

  virtual void setTarget(const SpritePtr& target);
  virtual void setSource(const SpritePtr& source);
};

class KnightMovementAnimation : public MovementAnimation {
protected:
  bool m_rotate;
  virtual boost::shared_ptr<Movement> createMovement(const QPoint& from, const QPoint& to) const;
public:
  KnightMovementAnimation(const SpritePtr& piece, const QPoint& destination,
                                              bool rotate, double speed = 1.0);
};

class CaptureAnimation : public OneShotAnimation {
protected:
  virtual void shoot();
public:
  CaptureAnimation(const SpritePtr& piece);
};

class DropAnimation : public OneShotAnimation {
  bool m_valid_position;
  QPoint m_position;
protected:
  virtual void shoot();
public:
  DropAnimation(const SpritePtr& piece);
  DropAnimation(const SpritePtr& piece, const QPoint&);
};

class PromotionAnimation : public OneShotAnimation {
protected:
  SpritePtr m_promoted;
  virtual void shoot();
public:
  PromotionAnimation(const SpritePtr& piece, const SpritePtr& promoted);
};

class DelayAnimation : public Animation {
  State m_state;

  int m_msecs;
  int m_start;
  void start();
public:
  DelayAnimation(int msecs);
  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort();
};

class FadeAnimation : public ConcreteAnimation {
  int m_fadeFrom;
  int m_fadeTo;
  QPoint m_to;
  State m_state;

  int m_start;
  void start();
public:
  FadeAnimation(const SpritePtr& sprite, const QPoint& pos, int fadeFrom, int fadeTo);
  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort();
};

class GrowAnimation : public ConcreteAnimation {
  State m_state;
  int m_start;

  void start();
public:
  GrowAnimation(const SpritePtr& sprite);
  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort();
};

class ExplodeAnimation : public ConcreteAnimation {
  State m_state;
  Random& m_random;

  int m_start;
  void start();
public:
  ExplodeAnimation(const SpritePtr& sprite, Random& random);
  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort();
};

/**
  * @brief Two groups of animations, to be executed one after the other.
  *
  * A double list of animations: pre-animations and post-animations.
  * When an AnimationGroup is advanced, pre-animations are handled first,
  * and removed from the list as they finish animating.
  * When all pre-animations are gone, post-animations are handled.
  */
class AnimationGroup : public Animation {
public:
  typedef boost::shared_ptr<Animation> AnimationPtr;
private:
  typedef std::list<AnimationPtr> AnimationList;
  typedef AnimationList::iterator Iterator;
  AnimationList pre;
  AnimationList post;

protected:
  bool m_active;
  bool m_persistent;
  bool m_chain_abortions;

  virtual void start();
public:
  AnimationGroup(bool persistent = false);

  virtual bool sticky() const { return m_persistent; }
  void setPersistent(bool value) { m_persistent = value; }
  void setChainAbortions(bool value) { m_chain_abortions = value; }

  void addPreAnimation(AnimationPtr);
  void addPostAnimation(AnimationPtr);

  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort() { stop(); }
};

class TeleportAnimation : public AnimationGroup {
public:
  TeleportAnimation(const SpritePtr& sprite, const QPoint& from, const QPoint& to);
  TeleportAnimation(const SpritePtr& sprite, const QPoint& to);
};

class CrossFadingAnimation : public AnimationGroup {
  SpritePtr m_piece;
protected:
  virtual void start();
public:
  CrossFadingAnimation(const SpritePtr& piece, const SpritePtr& promoted);
};

class DelayedAnimationSet : public Animation {
  class Event {
    int m_index;
    int m_time;
  public:
    Event(int index, int time)
    : m_index(index), m_time(time) { }
    int index() const { return m_index; }
    int time() const { return m_time; }

    bool operator<(const Event& other) const { return m_time < other.m_time; }
  };

  int m_time;
  int m_start;
  Random& m_random;
  State m_state;
  std::vector<boost::shared_ptr<Animation> > m_animations;
  AnimationGroup m_group;
  std::vector<Event> m_events;
  std::vector<Event>::iterator m_next_event;

  void execute(int);
public:
  DelayedAnimationSet(Random& random);
  void addAnimation(const boost::shared_ptr<Animation>& animation);
  void start();
  virtual State animationAdvance(int msec);
  virtual void stop();
  virtual void abort();
};

#endif // ANIMATION_H
