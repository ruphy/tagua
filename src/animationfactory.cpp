#include "animationfactory.h"

#include "animation.h"
#include "namedsprite.h"
#include "pointconverter.h"
#include "graphicalapi.h"

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

AnimationPtr move::run(const PointConverter* converter, AnimationType type) const {
  switch (type) {
  case Normal: {
    MovementAnimation* mov;
    QPoint destination = converter->toReal(m_to);
    Point origin = converter->toLogical(m_sprite.sprite()->pos() + 
      Point(converter->squareSize(), converter->squareSize()) / 2);
    if ((m_type & LShaped) && origin != m_to) {
      mov = new KnightMovementAnimation(m_sprite.sprite(), destination, m_type & Rotating);
    }
    else {
      mov = new MovementAnimation(m_sprite.sprite(), destination, m_type & Rotating);
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

AnimationPtr appear::run(const PointConverter*, AnimationType type) const {
  switch (type) {
  case Normal:
    return AnimationPtr(new FadeAnimation(m_sprite.sprite(), 0, 255));
  case Instant:
  default:
    return AnimationPtr(new DropAnimation(m_sprite.sprite()));
  }
}

disappear::disappear(const NamedSprite& sprite)
: m_sprite(sprite) { }

AnimationPtr disappear::run(const PointConverter*, AnimationType type) const {
  switch (type) {
  case Normal:
    return AnimationPtr(new FadeAnimation(m_sprite.sprite(), 255, 0));
  case Instant:
  default:
    return AnimationPtr(new CaptureAnimation(m_sprite.sprite()));
  }
}

destroy::destroy(const NamedSprite& sprite)
: m_sprite(sprite) { }

AnimationPtr destroy::run(const PointConverter*, AnimationType type) const {
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

AnimationPtr morph::run(const PointConverter*, AnimationType type) const {
  switch (type) {
  case Normal:
    return AnimationPtr(new CrossFadingAnimation(m_sprite.sprite(), m_new_sprite.sprite()));
  case Instant:
  default:
    return AnimationPtr(new PromotionAnimation(m_sprite.sprite(), m_new_sprite.sprite()));
  }
}

}

