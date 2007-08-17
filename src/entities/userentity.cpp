/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "userentity.h"

UserEntity::UserEntity(const GamePtr& game)
: Entity(game)
, m_editing_tools(false) { }

void UserEntity::enableEditingTools(bool value) {
  m_editing_tools = value;
}

NormalUserMove UserEntity::createMove(const Point& from, const Point& to) const {
  return NormalUserMove(from, to);
}

DropUserMove UserEntity::createDrop(int pool, int index, const Point& to) const {
  return DropUserMove(pool, index, to);
}

void UserEntity::handleRightClick(const Point&) const { }

bool UserEntity::canDetach() const { return true; }

