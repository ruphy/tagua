/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "entitytoken.h"

EntityToken::EntityToken(const boost::shared_ptr<Entity>& entity)
: m_entity(entity) { }

EntityToken::EntityToken() { }

boost::shared_ptr<Entity> EntityToken::entity() const { return m_entity.lock(); }

bool EntityToken::valid() const { return entity(); }
