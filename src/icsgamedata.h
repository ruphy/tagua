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

class ICSListener;

struct ICSGameData {
  int index;
  QString variant;
  boost::weak_ptr<ICSListener> listener;
  
  ICSGameData()
  : index(0) { }
  ICSGameData(int index, const QString& variant)
  : index(index), variant(variant) { }
};

#endif // ICSGAMEDATA_H
