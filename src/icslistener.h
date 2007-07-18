/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSLISTENER_H
#define ICSLISTENER_H

#include "tagua.h"

class ICSListener {
public:
  virtual ~ICSListener() { }
  virtual void notifyStyle12(const class PositionInfo&, bool is_starting=false) = 0;
  virtual void notifyPool(const class PoolInfo&) = 0;
  virtual void notifyMoveList(int, AbstractPosition::Ptr, const class PGN&) = 0;
};

#endif // ICSLISTENER_H
