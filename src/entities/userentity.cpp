/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "userentity.h"

UserEntity::UserEntity(int promotionType)
: m_editing_tools(false)
, m_promotion(promotionType) { }

void UserEntity::enableEditingTools(bool value) {
  m_editing_tools = value;
}

void UserEntity::handleRightClick(const Point&) const { }

bool UserEntity::canDetach() const { return true; }

