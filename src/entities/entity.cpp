/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "entity.h"
#include "game.h"

Entity::Entity(const GamePtr& game)
: m_premove(true)
, m_enabled(true)
, m_highlight(true)
, m_game(game) { }

Entity::~Entity() { }

GamePtr Entity::game() const {
  return m_game;
}

PositionPtr Entity::position() const {
  return m_game->position();
}

