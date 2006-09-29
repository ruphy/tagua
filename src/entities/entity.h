/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENTITY_H
#define ENTITY_H

#include "stdio.h"
#include "usermove.h"

class Entity {
protected:
  bool m_premove : 1;
  bool m_enabled : 1;
  bool m_highlight : 1;
public:
  enum Action {
    NoAction,
    Moving,
    Premoving
  };

  Entity();
  /**
    * Detach from dependent resource disposing it.
    */
  virtual ~Entity();

  bool highlight() const { return m_highlight; }
  void setHighlight(bool highlight) { m_highlight = highlight; }

  void setPremove(bool premove) { m_premove = premove; }
  void setEnabled(bool enabled) { m_enabled = enabled; }

  virtual bool canDetach() const = 0;
  /**
    * Detach from dependent resource without disposing it.
    */
  virtual void detach() { }
};

#endif // ENTITY_H
