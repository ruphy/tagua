/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "namedsprite.h"

NamedSprite::NamedSprite() { }

NamedSprite NamedSprite::duplicate() const {
  if (m_sprite) {
    return NamedSprite(m_name, SpritePtr(m_sprite->duplicate()));
  }
  else {
    return *this;
  }
}

NamedSprite::NamedSprite(const QString& name, const SpritePtr& s)
: m_name(name)
, m_sprite(s) { }


