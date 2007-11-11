/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "animationfactory.h"

#include "animation.h"
#include "namedsprite.h"
#include "pointconverter.h"
#include "indexconverter.h"
#include "graphicalapi.h"
#include "mastersettings.h"

namespace Common {
  AnimationPtr appear(const AnimationSettings& s, const NamedSprite& sprite, Animate::AnimationType type) {
    if (!s.fading)
      type = Animate::Instant;
      
    switch (type) {
    case Animate::Normal:
      return AnimationPtr(new FadeAnimation(sprite.sprite(), 0, 255));
    case Animate::Instant:
    default:
      return AnimationPtr(new DropAnimation(sprite.sprite()));
    }
  }
  
  AnimationPtr disappear(const AnimationSettings& s, const NamedSprite& sprite, Animate::AnimationType type) {
    if (!s.fading)
      type = Animate::Instant;
      
    switch (type) {
    case Animate::Normal:
      return AnimationPtr(new FadeAnimation(sprite.sprite(), 255, 0));
    case Animate::Instant:
    default:
      return AnimationPtr(new CaptureAnimation(sprite.sprite()));
    }
  }
};

AnimationSettings::AnimationSettings() {
  reload();
}

void AnimationSettings::reload() {
  Settings s = settings().group("animations");
  
  enabled = s.flag("enabled", true);
  maxSequence = 
      s.group("sequence").flag("enabled", true) 
    ? s.group("sequence")["max"].value<int>()
    : 0;
  movement = s["movement"].flag("enabled", true);
  explode = s["explode"].flag("enabled", true);
  fading = s["fading"].flag("enabled", true);
  transform = s["transform"].flag("enabled", true);
}

AnimationFactory::AnimationFactory(GraphicalAPI* api)
: m_api(api) {
  m_group = AnimationGroupPtr(new AnimationGroup);
}

AnimationGroupPtr AnimationFactory::group() const {
  return m_group;
}

void AnimationFactory::setGroup(const AnimationGroupPtr& group) {
  m_group = group;
}

void AnimationFactory::addPreAnimation(const Animate::Scheme& scheme, Animate::AnimationType type) {
  m_group->addPreAnimation(m_api->animate(scheme, type));
}

void AnimationFactory::addPostAnimation(const Animate::Scheme& scheme, Animate::AnimationType type) {
  m_group->addPostAnimation(m_api->animate(scheme, type));
}

AnimationFactory::operator AnimationGroupPtr() const {
  return group();
}

namespace Animate {
  
  Scheme::~Scheme() { }
  
  move::move(const NamedSprite& sprite, const Point& to, int type)
  : m_sprite(sprite)
  , m_to(to)
  , m_type(type) { }
  
  AnimationPtr move::run(const AnimationSettings& s, const PointConverter* converter, AnimationType type) const {
    int mov_type = m_type;
    if (!s.movement)
      type = Instant;
    else if (!s.transform) {
      mov_type &= ~Rotating;
    }
      
    switch (type) {
    case Normal: {
      MovementAnimation* mov;
      QPoint destination = converter->toReal(m_to);
      Point origin = converter->toLogical(m_sprite.sprite()->pos() + 
        Point(converter->squareSize(), converter->squareSize()) / 2);
      if ((mov_type & LShaped) && origin != m_to) {
        mov = new KnightMovementAnimation(m_sprite.sprite(), destination, mov_type & Rotating);
      }
      else {
        mov = new MovementAnimation(m_sprite.sprite(), destination, mov_type & Rotating);
      }
      return AnimationPtr(mov);
    }
    case Instant:
    default:
      return AnimationPtr(new InstantAnimation(m_sprite.sprite(), converter->toReal(m_to)));
    }
  }
  
  appear::appear(const NamedSprite& sprite)
  : m_sprite(sprite) { }
  
  AnimationPtr appear::run(const AnimationSettings& s, const PointConverter*, AnimationType type) const {
    return Common::appear(s, m_sprite, type);
  }
  
  disappear::disappear(const NamedSprite& sprite)
  : m_sprite(sprite) { }
  
  AnimationPtr disappear::run(const AnimationSettings& s, const PointConverter*, AnimationType type) const {
    return Common::disappear(s, m_sprite, type);
  }
  
  destroy::destroy(const NamedSprite& sprite)
  : m_sprite(sprite) { }
  
  AnimationPtr destroy::run(const AnimationSettings& s, const PointConverter*, AnimationType type) const {
    if (!s.explode) {
      return Common::disappear(s, m_sprite, type);
    }
    
    switch (type) {
    case Normal:
      return AnimationPtr(new ExplodeAnimation(m_sprite.sprite(), Random::instance()));
    case Instant:
    default:
      return AnimationPtr(new CaptureAnimation(m_sprite.sprite()));
    }
  }
  
  morph::morph(const NamedSprite& sprite, const NamedSprite& new_sprite)
  : m_sprite(sprite)
  , m_new_sprite(new_sprite) { }
  
  AnimationPtr morph::run(const AnimationSettings& s, const PointConverter*, AnimationType type) const {
    if (!s.fading)
      type = Instant;
      
    switch (type) {
    case Normal:
      return AnimationPtr(new CrossFadingAnimation(m_sprite.sprite(), m_new_sprite.sprite()));
    case Instant:
    default:
      return AnimationPtr(new PromotionAnimation(m_sprite.sprite(), m_new_sprite.sprite()));
    }
  }
  
  namespace Pool {
    
    move::move(const NamedSprite& sprite, int to)
    : m_sprite(sprite)
    , m_to(to) { }
    
    AnimationPtr move::run(const AnimationSettings& s, const IndexConverter* converter, AnimationType type) const {
      if (!s.movement)
        type = Instant;
        
      switch (type) {
      case Normal:
        return AnimationPtr(new MovementAnimation(m_sprite.sprite(), converter->toReal(m_to)));
      case Instant:
      default:
        return AnimationPtr(new InstantAnimation(m_sprite.sprite(), converter->toReal(m_to)));
      }
    }
    
    appear::appear(const NamedSprite& sprite)
    : m_sprite(sprite) { }
    
    AnimationPtr appear::run(const AnimationSettings& s, const IndexConverter*, AnimationType type) const {
      return Common::appear(s, m_sprite, type);
    }
    
    disappear::disappear(const NamedSprite& sprite)
    : m_sprite(sprite) { }
    
    AnimationPtr disappear::run(const AnimationSettings& s, const IndexConverter*, AnimationType type) const {
      return Common::disappear(s, m_sprite, type);
    }
    
  }

}

