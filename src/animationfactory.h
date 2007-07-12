/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ANIMATIONFACTORY_H
#define ANIMATIONFACTORY_H

#include "animation.h"
#include "fwd.h"
#include "point.h"

class NamedSprite;
class PointConverter;
class GraphicalAPI;

namespace Animate {

enum AnimationType {
  Normal,
  Instant
};

class Scheme {
public:
  virtual ~Scheme();
  virtual AnimationPtr run(const PointConverter*, AnimationType) const = 0;
};

}

class AnimationFactory {
  AnimationGroupPtr m_group;
  GraphicalAPI* m_api;
public:
  AnimationFactory(GraphicalAPI* api);
  
  AnimationGroupPtr group() const;
  
  void addPreAnimation(const Animate::Scheme& scheme, Animate::AnimationType type = Animate::Normal);
  void addPostAnimation(const Animate::Scheme& scheme, Animate::AnimationType type = Animate::Normal);
  
  operator AnimationGroupPtr() const;
};


namespace Animate {

class move : public Scheme {
	const NamedSprite& m_sprite;
	Point m_to;
public:
	move(const NamedSprite& sprite, const Point& to);
	virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
};

class appear : public Scheme {
	const NamedSprite& m_sprite;
public:
	appear(const NamedSprite& sprite);
	virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
};

class disappear : public Scheme {
	const NamedSprite& m_sprite;
public:
	disappear(const NamedSprite& sprite);
	virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
};

class destroy : public Scheme {
	const NamedSprite& m_sprite;
public:
	destroy(const NamedSprite& sprite);
	virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
};

class morph : public Scheme {
	const NamedSprite& m_sprite;
	const NamedSprite& m_new_sprite;
public:
	morph(const NamedSprite& sprite, const NamedSprite& new_sprite);
	virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
};



}


#endif // ANIMATIONFACTORY_H
