/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LUAAPI__LUAVALUE_H
#define LUAAPI__LUAVALUE_H

#include <boost/variant.hpp>
#include <QString>
#include <QMap>

class QPointF;
class QRectF;
class lua_State;

namespace LuaApi {

typedef boost::variant<double, QPointF, QRectF> LuaValue;
typedef QMap<QString, LuaValue> LuaValueMap;
void lua_pushvalue(lua_State* l, const LuaValue& value);
void lua_pushvaluemap(lua_State* l, const LuaValueMap* valuemap);

} //end namespace LuaApi

#endif //LUAAPI__LUAVALUE_H
