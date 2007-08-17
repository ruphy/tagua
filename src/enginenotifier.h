/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENGINENOTIFIER_H
#define ENGINENOTIFIER_H

#include <boost/enable_shared_from_this.hpp>

class QString;

class EngineNotifier : public boost::enable_shared_from_this<EngineNotifier> {
public:
  virtual ~EngineNotifier() { }
  virtual void notifyEngineMove(const QString&) = 0;
};

#endif // ENGINENOTIFIER_H
