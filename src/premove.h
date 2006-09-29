/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREMOVE_H
#define PREMOVE_H

#include "kboard.h"

class Premove {
protected:
  AbstractMove::Ptr m_move;
public:
  Premove(AbstractMove::Ptr move);
  virtual ~Premove() { }
  
  virtual AbstractMove::Ptr execute(AbstractPosition::Ptr);
  virtual class NormalUserMove toUserMove() const;
};

#endif // PREMOVE_H
