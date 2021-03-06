/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef FWD_H
#define FWD_H

#include <boost/shared_ptr.hpp>

typedef boost::shared_ptr<class AbstractMove> MovePtr;
typedef boost::shared_ptr<class AbstractPosition> PositionPtr;
typedef boost::shared_ptr<class AbstractPool> PoolPtr;
typedef boost::shared_ptr<class AbstractPiece> PiecePtr;
typedef boost::shared_ptr<class AbstractAnimator> AnimatorPtr;
typedef boost::shared_ptr<class ICSAPI> ICSAPIPtr;
typedef boost::shared_ptr<class VariantInfo> VariantPtr;

template <typename Variant> class UnwrappedGraphicalAPI;
template <typename Variant>
struct UnwrappedGraphicalAPIPtr {
  typedef boost::shared_ptr<UnwrappedGraphicalAPI<Variant> > type;
};

namespace HLVariant {
  template <typename Variant> class UnwrappedGraphicalAPI;
  template <typename Variant>
  struct UnwrappedGraphicalAPIPtr {
    typedef boost::shared_ptr<UnwrappedGraphicalAPI<Variant> > type;
  };
}

typedef boost::shared_ptr<class Animation> AnimationPtr;
typedef boost::shared_ptr<class AnimationGroup> AnimationGroupPtr;
typedef boost::shared_ptr<class DropAnimation> DropAnimationPtr;
typedef boost::shared_ptr<class PromotionAnimation> PromotionAnimationPtr;
typedef boost::shared_ptr<class CaptureAnimation> CaptureAnimationPtr;
typedef boost::shared_ptr<class InstantAnimation> InstantAnimationPtr;
typedef boost::shared_ptr<class DelayAnimation> DelayAnimationPtr;
typedef boost::shared_ptr<class MovementAnimation> MovementAnimationPtr;
typedef boost::shared_ptr<class FadeAnimation> FadeAnimationPtr;
typedef boost::shared_ptr<class GrowAnimation> GrowAnimationPtr;
typedef boost::shared_ptr<class ExplodeAnimation> ExplodeAnimationPtr;
typedef boost::shared_ptr<class TeleportAnimation> TeleportAnimationPtr;
typedef boost::shared_ptr<class CrossFadingAnimation> CrossFadingAnimationPtr;

namespace Animate { class Scheme; }
typedef boost::shared_ptr<Animate::Scheme> SchemePtr;

typedef boost::shared_ptr<class Sprite> SpritePtr;

typedef boost::shared_ptr<class Game> GamePtr;

#endif //FWD_H
