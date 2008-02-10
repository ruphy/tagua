/*
  Copyright (c) 2006-2008 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "luavalue.h"
#include <KDebug>
#include "imaging.h"

namespace LuaApi {

void lua_pushvalue(lua_State* l, const LuaValue& value) {
  if(const double* v = boost::get<double>(&value))
    lua_pushnumber(l, *v);
  else if(const QPointF* v = boost::get<QPointF>(&value))
    Wrapper<QPointF>::create(l, *v);
  else if(const QRectF* v = boost::get<QRectF>(&value))
    Wrapper<QRectF>::create(l, *v);
  else
    kError() << "Unknown variant type!";
}

void lua_pushvaluemap(lua_State* l, const LuaValueMap* valuemap) {
  lua_newtable(l); // metatable

  for(LuaValueMap::const_iterator it = valuemap->begin(); it != valuemap->end(); ++it) {
    lua_pushvalue(l, it.value());
    lua_setfield(l, -2, it.key().toAscii().constData());
  }
}

} //end namespace LuaApi
