#include "animationfactory.h"

#include "animation.h"
#include "namedsprite.h"
#include "pointconverter.h"

namespace Animate {

Scheme::~Scheme() { }

move::move(const NamedSprite& sprite, const Point& to)
: m_sprite(sprite)
, m_to(to) { }

AnimationPtr move::run(PointConverter* converter, AnimationType type) {
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

AnimationPtr appear::run(PointConverter*, AnimationType type) {
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

AnimationPtr disappear::run(PointConverter*, AnimationType type) {
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

AnimationPtr destroy::run(PointConverter*, AnimationType type) {
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

AnimationPtr morph::run(PointConverter*, AnimationType type) {
	switch (type) {
	case Normal:
		return AnimationPtr(new CrossFadingAnimation(m_sprite.sprite(), m_new_sprite.sprite()));
	case Instant:
	default:
		return AnimationPtr(new PromotionAnimation(m_sprite.sprite(), m_new_sprite.sprite()));
	}
}

}

