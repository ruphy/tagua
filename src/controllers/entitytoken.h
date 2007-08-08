/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENTITYTOKEN_H
#define ENTITYTOKEN_H

#include <set>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

class Entity;

class EntityToken {
  boost::weak_ptr<Entity> m_entity;
public:
  EntityToken(const boost::shared_ptr<Entity>& entity);
  EntityToken();
  
  boost::shared_ptr<Entity> entity() const;
  bool valid() const;
};

#endif // ENTITYTOKEN_H
