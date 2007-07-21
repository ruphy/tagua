/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef NAMEDSPRITE_H
#define NAMEDSPRITE_H

#include <QString>
#include "sprite.h"
#include "fwd.h"

class NamedSprite {
public:
  QString m_name;
  SpritePtr m_sprite;

  NamedSprite();
  NamedSprite(const QString& name, const SpritePtr& s);
  
  QString name() const { return m_name; }
  SpritePtr sprite() const { return m_sprite; }
  operator bool() const { return !!m_sprite; }
  
  NamedSprite duplicate() const;
};

#endif //NAMEDSPRITE_H
