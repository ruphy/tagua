/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSGAMEDATA_H
#define ICSGAMEDATA_H

#include <boost/weak_ptr.hpp>
#include <QString>

#include "fwd.h"

class ICSListener;

struct ICSGameData {
  int index;
  VariantPtr variant;
  ICSAPIPtr icsapi;
  boost::weak_ptr<ICSListener> listener;
  
  ICSGameData();
  ICSGameData(int index, const QString& type);
  
  void setType(const QString& var);
};

#endif // ICSGAMEDATA_H
