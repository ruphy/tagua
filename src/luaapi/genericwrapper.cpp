/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QDir>
#include "genericwrapper.h"

namespace LuaApi {

QString GenericWrapperBase::file_path(lua_State* l, const QString& f) {
  StackCheck check(l);

  lua_getfield(l, LUA_REGISTRYINDEX, CURRENT_DIRECTORY);
  QDir* dir = reinterpret_cast<QDir*>(lua_touserdata(l, -1));
  lua_pop(l, 1);
  return QDir::cleanPath(QDir::isAbsolutePath(f) ? f : dir->filePath(f));
}

Loader::Context* GenericWrapperBase::retrieve_context(lua_State* l) {
  StackCheck check(l);

  lua_getfield(l, LUA_REGISTRYINDEX, LOADING_CONTEXT);
  Loader::Context* retv = reinterpret_cast<Loader::Context*>(lua_touserdata(l, -1));
  lua_pop(l, 1);
  return retv;
}

void GenericWrapperBase::set_property(lua_State* l, const char* name, lua_CFunction get,
                                                        lua_CFunction set, int index) {
  StackCheck check(l);

  lua_getfield(l,index,".get");
  lua_pushcfunction(l, get);
  lua_setfield(l, -2, name);
  lua_pop(l, 1);

  if(set) {
    lua_getfield(l,index,".set");
    lua_pushcfunction(l, set);
    lua_setfield(l, -2, name);
    lua_pop(l, 1);
  }
}

void GenericWrapperBase::set_method(lua_State* l, lua_CFunction f, const char* name,
                                                                      int index) {
  StackCheck check(l);

  lua_getfield(l,index,".methods");
  lua_pushcfunction(l, f);
  lua_setfield(l, -2, name);
  lua_pop(l, 1);
}

void GenericWrapperBase::set_meta_method(lua_State* l, lua_CFunction f,
                                              const char* name, int index) {
  StackCheck check(l);

  lua_pushcfunction(l, f);
  lua_setfield(l, index-1, name);
}

int GenericWrapperBase::object_meta_index_event(lua_State* l) {

  /* should i check the object before the meta table? no... */
  lua_getmetatable(l,-2);

  //key, MT
  lua_pushstring(l,".methods");
  lua_rawget(l,-2);
  lua_pushvalue(l,-3);
  lua_gettable(l,-2);

  //key, MT, .methods, method
  if(!lua_isnil(l,-1)) {
    lua_remove(l,-2);
    lua_remove(l,-2);
    lua_remove(l,-2);
    return 1;
  }
  lua_pop(l,2);

  //key, MT
  lua_pushstring(l,".get");
  lua_rawget(l,-2);
  lua_pushvalue(l,-3);
  lua_gettable(l,-2);

  //key, MT, .get, getter
  if(!lua_isnil(l,-1)) {
    lua_remove(l,-2);
    lua_remove(l,-2);
    lua_remove(l,-2);
    lua_pushvalue(l,-2);
    lua_call(l, 1, 1);
    return 1;
  }

  luaL_error(l, "Can't get unexisting method/property %s\n", lua_tostring(l,-4));
  lua_pop(l,4);
  return 0;
}

int GenericWrapperBase::object_meta_newindex_event(lua_State* l) {
  StackCheck check(l, -2);

  /* should i check the object before the meta table? no... */
  lua_getmetatable(l,-3);

  //key, value, MT
  lua_pushstring(l,".set");
  lua_rawget(l,-2);
  lua_pushvalue(l,-4);
  lua_gettable(l,-2);

  //key, value, MT, .set, setter
  if(!lua_isnil(l,-1)) {
    lua_remove(l,-2);
    lua_remove(l,-2);
    lua_remove(l,-3);
    lua_pushvalue(l,-3);
    lua_pushvalue(l,-3);
    lua_remove(l,-4);
    lua_call(l, 2, 0);
    return 0;
  }

  luaL_error(l, "Can't set unexisting property %s\n", lua_tostring(l,-5));
  lua_pop(l, 5);
  return 0;
}


} //end namespace LuaApi
