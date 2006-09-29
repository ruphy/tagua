/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GAMEBASEDENTITY_H
#define GAMEBASEDENTITY_H

#include "kboard.h"

class Game;

class GameBasedEntity {
protected:
  boost::shared_ptr<Game> m_game;
public:
  GameBasedEntity(const boost::shared_ptr<Game>& game);

  boost::shared_ptr<Game> game() const { return m_game; }
  AbstractPosition::Ptr position() const;
};

#endif // GAMEBASEDENTITY_H
