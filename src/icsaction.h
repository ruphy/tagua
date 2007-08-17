/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSACTION_H
#define ICSACTION_H

class ICSAction {
public:
  enum Direction {
    Forward,
    Back
  };
private:
  Direction m_direction;
  int m_moves;
  QString m_actor;
public:
  ICSAction(const Direction& direction, int moves, const QString& actor)
  : m_direction(direction)
  , m_moves(moves)
  , m_actor(actor) { }
  
  Direction direction() const { return m_direction; }
  int moves() const { return m_moves; }
  QString actor() const { return m_actor; }
};

#endif // ICSACTION_H
