/*
  Copyright (c) 2006-2008 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "icsgamedata.h"
#include <KDebug>
#include "tagua.h"
#include "gameinfo.h"
#include "variants.h"

ICSGameData::ICSGameData()
: index(0) {
  setType("");
}

ICSGameData::ICSGameData(int index, const QString& type)
: index(index) {
  setType(type);
}

void ICSGameData::setType(const QString& type) {
  variant = Variants::instance().get(GameInfo::variantCode(type));
  if (!variant) {
    kError() << "No variant corresponding to " << type;
  }
  else if (!(icsapi = variant->icsAPI())) {
    // There's no ICSAPI in this variant.
    // This means we cannot use it for ICS stuff, so we
    // fall back to the dummy variant.
    // TODO
  }  
}
