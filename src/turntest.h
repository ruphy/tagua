/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef TURNTEST_H
#define TURNTEST_H

class TurnTest {
public:
  virtual ~TurnTest() { }
  virtual bool operator()(int turn) const = 0;
};

#endif // TURNTEST_H
