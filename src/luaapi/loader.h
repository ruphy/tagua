/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LUAAPI__LOADER_H
#define LUAAPI__LOADER_H

#include <map>
#include <vector>
#include <boost/variant.hpp>
#include <QDir>
#include <QImage>
#include "loader/image.h"
#include "luaapi/luavalue.h"
#include "option.h"

class lua_State;
class luaL_Reg;

namespace Loader {
  class Context;
}

namespace LuaApi {

class RectLess {
public:
  bool operator()(const QRect& a, const QRect& b) {
    return a.x() != b.x() ? a.x() < b.x() :
           a.y() != b.y() ? a.y() < b.y() :
           a.width() != b.width() ? a.width() < b.width() :
           a.height() < b.height();
  }
};

typedef std::map<QRect, QImage, RectLess> ImageMap;
typedef boost::variant<QImage, ImageMap> ImageOrMap;

class Loader {
  bool m_error;
  QString m_error_string;

  lua_State* m_state;
  QDir m_curr_dir;

  static const luaL_Reg lualibs[];

  template<typename T> struct create_value_data;
  template<typename T> static void retrieve(create_value_data<T>*, lua_State *l, int pos);
  template<typename T> static int create_value_func(lua_State *l);

  static int import_func(lua_State *l);

public:
  Loader(::Loader::Context *ctx);
  ~Loader();

  lua_State* state() const { return m_state; }
  bool runFile(const QString& file, bool set_dir = true);

  template<typename T> T getValue(const QString& key, int size = 0,
                            const LuaValueMap* args = NULL, bool allow_nil = false);

  bool error(){ return m_error; }
  void clearError(){ m_error = false; }
  QString errorString(){ return m_error_string; }
};

} //end namespace LuaApi


#endif // LUAAPI__LOADERAPI_H
