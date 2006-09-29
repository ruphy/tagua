/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PLAYER_H
#define PLAYER_H

#include <qstring.h>

class Player {
public:
  QString name;
  int rating;
  
  Player(const QString& name, int rating)
  : name(name)
  , rating(rating) {  }
  
  Player()
  : name("unknown")
  , rating(-1) { }
  
  bool operator==(const QString& otherName) const {
    return name == otherName;
  }
};

#endif // PLAYER_H
