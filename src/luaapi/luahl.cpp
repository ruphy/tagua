/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "luahl.h"
#include "genericwrapper.h"
#include <QColor>
#include <QFile>
#include "common.h"
#include "hline.h"
#include "imaging.h"

#define ARG_CHECK(exp, func) \
    if (n != (exp)) luaL_error(l, "Wrong argument count for %s::%s", class_name() ,#func)

namespace LuaApi {

template <>
class Wrapper<HLine> : public GenericWrapper<HLine> {
public:
  static const char* class_name() {
    return "HLine";
  }

  PROPERTY_RO(length, length, number);

  static void create_index_table(lua_State* l) {
    SET_PROPERTY_RO(l, length);
    set_method(l, &mid, "mid");
    set_method(l, &set_bold, "set_bold");
    set_method(l, &set_italic, "set_italic");
    set_method(l, &set_color, "set_color");
    set_method(l, &clone, "clone");
    set_method(l, &extract, "extract");
    set_method(l, &append, "append");
    set_method(l, &dump, "dump");
  }

  static int constructor(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(1, HLine);
    const char* str = lua_tostring(l, 1);
    lua_pop(l, n);

    HLine* hline = new HLine(str, QTextCharFormat());
    allocate(l, hline);
    return 1;
  }

  /**
    * Release the object at @a index from Lua's ownership.
    */
  static void release(lua_State* l, int index) {
    StackCheck check(l);

    lua_getmetatable(l, index);
    lua_pushstring(l, "__gc");
    lua_pushnil(l);
    lua_settable(l, -3);
    lua_pop(l, 1);
  }

  static int clone(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(1, clone);

    HLine* self = retrieve(l, 1, AssertOk);
    lua_pop(l, n);

    allocate(l, new HLine(*self));
    return 1;
  }

  static int extract(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(3, extract);

    HLine* self = retrieve(l, 1, AssertOk);
    int begin = static_cast<int>(lua_tonumber(l, 2));
    int end = static_cast<int>(lua_tonumber(l, 3));
    lua_pop(l, n);

    allocate(l, self->extract(begin, end));
    return 1;
  }

  static int append(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(4, append);

    HLine* self = retrieve(l, 1, AssertOk);
    HLine* other = retrieve(l, 2, AssertOk);
    int begin = static_cast<int>(lua_tonumber(l, 3));
    int end = static_cast<int>(lua_tonumber(l, 4));
    lua_pop(l, n);

    allocate(l, self->append(*other, begin, end));
    return 1;
  }

  static int mid(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(3, mid);

    HLine* self = retrieve(l, 1, AssertOk);
    int from = static_cast<int>(lua_tonumber(l, 2));
    int to = static_cast<int>(lua_tonumber(l, 3));
    lua_pop(l, n);

    lua_pushstring(l, qPrintable(self->mid(from, to)));
    return 1;
  }

  static int set_bold(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(4, set_bold);

    HLine* self = retrieve(l, 1, AssertOk);
    int from = static_cast<int>(lua_tonumber(l, 2));
    int to = static_cast<int>(lua_tonumber(l, 3));
    bool value = lua_toboolean(l, 4);
    lua_pop(l, n);

    self->setBold(from, to, value);
    return 0;
  }

  static int set_italic(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(4, set_italic);

    HLine* self = retrieve(l, 1, AssertOk);
    int from = static_cast<int>(lua_tonumber(l, 2));
    int to = static_cast<int>(lua_tonumber(l, 3));
    bool value = lua_toboolean(l, 4);
    lua_pop(l, n);

    self->setItalic(from, to, value);
    return 0;
  }

  static int set_color(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(4, set_bold);

    HLine* self = retrieve(l, 1, AssertOk);
    int from = static_cast<int>(lua_tonumber(l, 2));
    int to = static_cast<int>(lua_tonumber(l, 3));
    QColor color = Wrapper<QColor>::get(l, 4);
    lua_pop(l, n);

    self->setColor(from, to, color);
    return 0;
  }

  static int dump(lua_State* l) {
    const int n = lua_gettop(l);
    ARG_CHECK(1, dump);

    HLine* self = retrieve(l, 1, AssertOk);
    lua_pop(l, n);

    self->dump();
    return 0;
  }

};



static const luaL_Reg lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {NULL, NULL}
};

LUALIB_API void luaL_openlibs (lua_State*) { }

Api::Api() {
  lua_State* l = lua_open();
  m_state = l;

  for (const luaL_Reg *lib = lualibs; lib->func; lib++) {
    lua_pushcfunction(l, lib->func);
    lua_pushstring(l, lib->name);
    lua_call(l, 1, 0);
  }

  Wrapper<QColor>::register_class(l);
  Wrapper<HLine>::register_class(l);
}

Api::~Api() {
  lua_close(m_state);
}

void Api::runFile(const char* file) {
  //luaL_dofile(m_state, file);
  if (QFile(file).exists()) {
    if(luaL_loadfile(m_state, file) == 0)
      pcall(0, LUA_MULTRET);
    else {
      kDebug() << "LOADFILE FOR " << file << " FAILED";
      kDebug() << lua_tostring(m_state, -1);
    }
  }
  else
    kDebug() << "FILE " << file << " DOES NOT EXIST";
}

void Api::pcall(int nArgs, int nResults) {
  if (lua_pcall(m_state, nArgs, nResults, 0) != 0) {
    kDebug() << "RUNTIME ERROR: " << lua_tostring(m_state, -1);
  }
}

#if 0
int Api::create_line(lua_State* l) {
  const char* line = lua_tostring(l, -1);
  lua_pop(l, 1);
  lua_getglobal(l, "Text");
  Wrapper<HLine>::allocate(l,
    new HLine(line, QTextCharFormat()));
  if (lua_pcall(l, 1, 1, 0) != 0)
    kDebug() << "ERROR INSIDE create_line";
  return 1;
}

void Api::loadTest() {

  lua_pushcfunction(m_state, &create_line);
  lua_setglobal(m_state, "create_line");
}
#endif

HLine* Api::highlight(const QString& text) {
  // iterate through patterns
  StackCheck check(m_state);
  lua_getglobal(m_state, "__patterns__");
  if (lua_istable(m_state, -1)) {
    lua_pushnil(m_state);
    while (lua_next(m_state, -2) != 0) {
      int i = static_cast<int>(lua_tonumber(m_state, -2));
      lua_pushstring(m_state, "pattern");
      lua_gettable(m_state, -2);
      QString pattern(lua_tostring(m_state, -1));
      QRegExp re(pattern.replace("%", "\\"));
      lua_pop(m_state, 2);

      std::pair<bool, HLine*> res = runEvent(text, i, re);
      if (res.first) {
        lua_pop(m_state, 2);
        return res.second;
      }

    }
  }

  lua_pop(m_state, 1);
  return new HLine(text, QTextCharFormat());
}

void Api::pushpair(int x, int y) {
  lua_newtable(m_state);
  lua_pushstring(m_state, "from");
  lua_pushnumber(m_state, x);
  lua_settable(m_state, -3);
  lua_pushstring(m_state, "to");
  lua_pushnumber(m_state, y);
  lua_settable(m_state, -3);
}

std::pair<bool, HLine*> Api::runEvent(const QString& text, int eventIndex, QRegExp& pattern) {
  StackCheck check(m_state);

  // match regexp against text
  int index = pattern.indexIn(text);
  if (index == -1) return std::pair<bool, HLine*>(false, 0);

  lua_getglobal(m_state, "__run_event__");

  if (!lua_isfunction(m_state, -1)) {
    kDebug() << "** ERROR: __run_event__ is corrupted (type = " <<
      lua_typename(m_state, lua_type(m_state, -1)) << ") **";
    lua_pop(m_state, 1);
    return std::pair<bool, HLine*>(false, 0);
  }

  // event
  lua_getglobal(m_state, "__patterns__");
  if (!lua_istable(m_state, -1)) {
    kDebug() << "** ERROR: __patterns__ is corrupted **";
    lua_pop(m_state, 2);
    return std::pair<bool, HLine*>(false, 0);
  }
  lua_pushnumber(m_state, eventIndex);
  lua_gettable(m_state, -2);
  lua_remove(m_state, -2);

  // str
  lua_pushstring(m_state, qPrintable(text));

  // match
  pushpair(index, index + pattern.matchedLength());

  // ref
  lua_newtable(m_state);
  const int n = pattern.numCaptures();
  for (int i = 1; i <= n; i++) {
    int pos = pattern.pos(i);
    lua_pushnumber(m_state, i);
    pushpair(pos, pos + pattern.cap(i).length());
    lua_settable(m_state, -3);
  }

  // state
  lua_newtable(m_state);
  // TODO: fill state

  pcall(5, 1);

  if (lua_isnil(m_state, -1)) {
    lua_pop(m_state, 1);
    return std::pair<bool, HLine*>(true, 0);
  }

  {
    lua_pushstring(m_state, "from");
    lua_gettable(m_state, -2);
    int from = static_cast<int>(lua_tonumber(m_state, -1));
    lua_pop(m_state, 1);

    lua_pushstring(m_state, "to");
    lua_gettable(m_state, -2);
    int to = static_cast<int>(lua_tonumber(m_state, -1));
    lua_pop(m_state, 1);

    lua_pushstring(m_state, "line");
    lua_gettable(m_state, -2);
    HLine* res = Wrapper<HLine>::retrieve(m_state, -1, AssertOk);

    if (from == 0 && to == 0)
      Wrapper<HLine>::release(m_state, -1);
    else
      res = res->extract(from, to);

    lua_pop(m_state, 2);

    return std::make_pair(true, res);
  }
}

} // namespace LuaApi

