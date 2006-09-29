/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENGINETOKEN_H
#define ENGINETOKEN_H

#include <set>
#include <boost/shared_ptr.hpp>

class Entity;

class EntityToken {
  typedef std::set<boost::shared_ptr<Entity> >::const_iterator Iterator;
  Iterator m_iterator;
  bool m_valid;
public:
  EntityToken(const Iterator& iterator)
  : m_iterator(iterator)
  , m_valid(true) { }
  EntityToken()
  : m_valid(false) { }
  const Iterator& iterator() const { return m_iterator; }
  bool valid() const { return m_valid; }
};

#endif // ENGINETOKEN_H
