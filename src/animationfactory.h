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

#include "fwd.h"
#include "point.h"
class NamedSprite;
class PointConverter;

// typedef boost::function<AnimationPtr()> AnimationScheme;

namespace Animate {

enum AnimationType {
	Normal,
	Instant
};

class Scheme {
public:
	virtual ~Scheme();
	virtual AnimationPtr run(PointConverter*, AnimationType) = 0;
};

class move : public Scheme {
	const NamedSprite& m_sprite;
	Point m_to;
public:
	move(const NamedSprite& sprite, const Point& to);
	virtual AnimationPtr run(PointConverter* converter, AnimationType type);
};

class appear : public Scheme {
	const NamedSprite& m_sprite;
public:
	appear(const NamedSprite& sprite);
	virtual AnimationPtr run(PointConverter* converter, AnimationType type);
};

class disappear : public Scheme {
	const NamedSprite& m_sprite;
public:
	disappear(const NamedSprite& sprite);
	virtual AnimationPtr run(PointConverter* converter, AnimationType type);
};

class destroy : public Scheme {
	const NamedSprite& m_sprite;
public:
	destroy(const NamedSprite& sprite);
	virtual AnimationPtr run(PointConverter* converter, AnimationType type);
};

class morph : public Scheme {
	const NamedSprite& m_sprite;
	const NamedSprite& m_new_sprite;
public:
	morph(const NamedSprite& sprite, const NamedSprite& new_sprite);
	virtual AnimationPtr run(PointConverter* converter, AnimationType type);
};



}


#endif // ANIMATIONFACTORY_H
