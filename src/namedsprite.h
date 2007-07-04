/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef NAMEDSPRITE_H
#define NAMEDSPRITE_H

#include <QString>
#include "sprite.h"

class NamedSprite {
public:
  QString m_name;
  SpritePtr m_sprite;

  NamedSprite() {}
  NamedSprite(const QString& name, boost::shared_ptr<Sprite> s)
    : m_name(name), m_sprite(s) {}
  QString name(){ return m_name; }
  SpritePtr sprite(){ return m_sprite; }
  operator bool(){ return !!m_sprite; }
};

#endif //NAMEDSPRITE_H
