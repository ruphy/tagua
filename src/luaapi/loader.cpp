/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include <stdlib.h>
#include <iostream>
#include <QRect>
#include <QDir>
#include "common.h"
#include "loader/image.h"
#include "luaapi/imaging.h"
#include "luaapi/loader.h"
#include "luaapi/options.h"

namespace LuaApi {

const luaL_Reg Loader::lualibs[] = {
  {"", luaopen_base},
  {LUA_LOADLIBNAME, luaopen_package},
  {LUA_TABLIBNAME, luaopen_table},
  {LUA_IOLIBNAME, luaopen_io},
  {LUA_OSLIBNAME, luaopen_os},
  {LUA_STRLIBNAME, luaopen_string},
  {LUA_MATHLIBNAME, luaopen_math},
  {LUA_DBLIBNAME, luaopen_debug},
  {LUA_FUNCLIBNAME, luaopen_func},
  {LUA_COLLECTLIBNAME, luaopen_collect},
  {NULL, NULL}
};

Loader::Loader(::Loader::Context *ctx)
: m_error(false) {
  lua_State* l = lua_open();
  m_state = l;

  for (const luaL_Reg *lib = lualibs; lib->func; lib++) {
    lua_pushcfunction(l, lib->func);
    lua_pushstring(l, lib->name);
    lua_call(l, 1, 0);
  }

  Wrapper<QRectF>::register_class(l);
  Wrapper<QPointF>::register_class(l);
  Wrapper<QColor>::register_class(l);
  Wrapper<QLinearGradient>::register_class(l);
  Wrapper<QRadialGradient>::register_class(l);
  Wrapper<QConicalGradient>::register_class(l);
  Wrapper<QBrush>::register_class(l);
  Wrapper< ::Loader::Image>::register_class(l);
  Wrapper< ::Loader::Glyph>::register_class(l);

  Wrapper<OptList>::register_class(l);
  Wrapper<BoolOptList>::register_class(l);
  Wrapper<BoolOptPtr>::register_class(l);
  Wrapper<IntOptPtr>::register_class(l);
  Wrapper<StringOptPtr>::register_class(l);
  Wrapper<UrlOptPtr>::register_class(l);
  Wrapper<ColorOptPtr>::register_class(l);
  Wrapper<FontOptPtr>::register_class(l);
  Wrapper<ComboOptPtr>::register_class(l);
  Wrapper<SelectOptPtr>::register_class(l);

  lua_pushlightuserdata(l, ctx);
  lua_setfield(l, LUA_REGISTRYINDEX, LOADING_CONTEXT);

  lua_pushlightuserdata(l, this);
  lua_setfield(l, LUA_REGISTRYINDEX, API_LOADER);

  lua_pushlightuserdata(m_state, &m_curr_dir);
  lua_setfield(m_state, LUA_REGISTRYINDEX, CURRENT_DIRECTORY);

  lua_newtable(l);
  lua_setglobal(l, "theme");

  lua_pushcfunction(l, import_func);
  lua_setglobal(l, "import");
}

Loader::~Loader() {
  lua_close(m_state);
}

bool Loader::runFile(const QString& file, bool setdir) {

  QString path = QDir::cleanPath(
                    QDir::isAbsolutePath(file) ? file : m_curr_dir.filePath(file) );
  if(setdir) {
    QFileInfo f_info( path );
    m_curr_dir = f_info.dir();
  }

  bool retv;
  if(luaL_loadfile(m_state, path.toAscii().constData()) == 0) {
    if(lua_pcall(m_state, 0, LUA_MULTRET, 0) != 0)
      retv = false;
    else
      retv = true;
  }
  else
    retv = false;

  if(!retv) {
    m_error = true;
    m_error_string = QString(lua_tostring(m_state, -1));
    lua_pop(m_state, 1);
  }

  return retv;
}

template<typename T>
struct Loader::create_value_data {
  const QString& key;
  int size;
  T out;
  create_value_data(const QString& _key, int _size)
    : key(_key)
    , size(_size) {
  }
};

template<typename T>
T Loader::getValue(const QString& key, int size) {
  StackCheck s(m_state);

  create_value_data<T> data(key, size);
  if(lua_cpcall(m_state, create_value_func<T>, &data) != 0) {
    m_error = true;
    m_error_string = QString(lua_tostring(m_state, -1))+"\nsearched key was: "+key;
    lua_pop(m_state, 1);
    return T();
  }
  return data.out;
}

template ::Loader::Glyph Loader::getValue< ::Loader::Glyph>(const QString& /*id*/, int /*size*/);
template OptList Loader::getValue<OptList>(const QString& /*id*/, int /*size*/);
template QString Loader::getValue<QString>(const QString& /*id*/, int /*size*/);
template QStringList Loader::getValue<QStringList>(const QString& /*id*/, int /*size*/);
template QImage Loader::getValue<QImage>(const QString& /*id*/, int /*size*/);
template ImageOrMap Loader::getValue<ImageOrMap>(const QString& /*id*/, int /*size*/);
template double Loader::getValue<double>(const QString& /*id*/, int /*size*/);
template QPointF Loader::getValue<QPointF>(const QString& /*id*/, int /*size*/);
template QRectF Loader::getValue<QRectF>(const QString& /*id*/, int /*size*/);
template QBrush Loader::getValue<QBrush>(const QString& /*id*/, int /*size*/);
template QColor Loader::getValue<QColor>(const QString& /*id*/, int /*size*/);

template<typename T>
void Loader::retrieve(create_value_data<T>* d, lua_State *l, int pos) {
  d->out = *Wrapper<T>::retrieve(l, pos, AssertOk);
}

template<>
void Loader::retrieve<double>(create_value_data<double>* d, lua_State *l, int pos) {
  d->out = lua_tonumber(l, pos);
}

template<>
void Loader::retrieve<QString>(create_value_data<QString>* d, lua_State *l, int pos) {
  d->out = lua_tostring(l, pos);
}

template<>
void Loader::retrieve<QStringList>(create_value_data<QStringList>* d, lua_State *l, int pos) {
  if(lua_isstring(l, pos))
    d->out << QString(lua_tostring(l, -1));
  else if(lua_istable(l, pos)) {
    lua_pushnil(l);
    while (lua_next(l, pos<0 ? pos-1 : pos) != 0) {
      d->out << QString(lua_tostring(l, -1));
      lua_pop(l, 1);
    }
  }
  else
    luaL_error(l, "Can't convert to a QStringList (not string nor table)");
}

template<>
void Loader::retrieve<QImage>(create_value_data<QImage>* d, lua_State *l, int pos) {
  ::Loader::Image *retv = Wrapper< ::Loader::Image>::retrieve(l, pos, AssertOk);
  d->out = retv->m_image;
}

template<>
void Loader::retrieve<ImageOrMap>(create_value_data<ImageOrMap>* d, lua_State *l, int pos) {
  if(::Loader::Image *img = Wrapper< ::Loader::Image>::retrieve(l, pos))
     d->out = img->m_image;
  else if(lua_istable(l, pos)) {

    //collect the images in this way to avoid leaking memory if Wrapper::retrieve raises an exception
    d->out = ImageMap();
    ImageMap& out = boost::get<ImageMap>(d->out);

    lua_pushnil(l);
    while (lua_next(l, pos<0 ? pos-1 : pos) != 0) {
      QRectF *rect = Wrapper<QRectF>::retrieve(l, -2, AssertOk);
      ::Loader::Image *img = Wrapper< ::Loader::Image>::retrieve(l, -1, AssertOk);

      QRect r = rect->toRect();
      out[rect->toRect()] = img->m_image;

      lua_pop(l, 1);
    }
  }
  else
    luaL_error(l, "Can't convert to a ImageOrMap (not image nor table)");
}

template<typename T>
int Loader::create_value_func(lua_State *l) {
  StackCheck s(l, -1);
  create_value_data<T>* data = reinterpret_cast<create_value_data<T>*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  lua_getglobal(l, "theme");
  lua_getfield(l, -1, data->key.toAscii().constData());
  lua_remove(l, -2);

  /* If it is a function call it, or else try to retrieve directly the value */
  if(lua_isfunction(l, -1)) {
    if(data->size) {
      lua_pushnumber(l, data->size);
      lua_call(l, 1, 1);
    }
    else
      lua_call(l, 0, 1);
  }

  retrieve<T>(data, l, -1);
  lua_pop(l, 1);

  return 0;
}


int Loader::import_func(lua_State *l) {
  lua_getfield(l, LUA_REGISTRYINDEX, API_LOADER);
  Loader* api = reinterpret_cast<Loader*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  int n = lua_gettop(l);
  if(n != 1)
    luaL_error(l, "Wrong argument count for \"import\"");
  QString file(lua_tostring(l, 1));
  lua_pop(l, n);

  if(!api->runFile(file, false))
    luaL_error(l, "Error importing \"%s\":\n%s", file.toAscii().constData(),
                                            wrap_cptr(lua_tostring(l, -1)));
  return 0;
}

} // namespace LuaLoader
