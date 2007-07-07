/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef AGENT_H
#define AGENT_H

#include "fwd.h"

class Agent {
protected:
  bool m_active;
public:
  virtual ~Agent() { }
  Agent() : m_active(true) { }
  virtual void notifyClockUpdate(int, int) = 0;
  virtual void notifyMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref) = 0;
  virtual void notifyBack() = 0;
  virtual void notifyForward() = 0;
  virtual void notifyGotoFirst() = 0;
  virtual void notifyGotoLast() = 0;
  virtual void stop() { m_active = false; }
  bool active() const { return m_active; }
};

#endif // AGENT_H
