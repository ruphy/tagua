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

#include <KConfigGroup>
#include <KDesktopFile>
#include <KStandardDirs>

#include "common.h"
#include "loader/image.h"
#include "luaapi/imaging.h"
#include "luaapi/loader.h"
#include "luaapi/options.h"
#include "themeinfo.h"

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

Loader::Loader()
: m_error(false) {
  initialize(0);
}

Loader::Loader(::Loader::Context *ctx, const ThemeInfo& theme)
: m_error(false) {
  initialize(ctx);
  addMetaData(theme);
}

void Loader::addMetaData(const ThemeInfo& theme) {
  lua_State* const l = m_state;
  
  lua_newtable(l);
  
#define ADD_FIELD_AUX(FIELD_NAME, FIELD) \
  lua_pushstring(l, FIELD_NAME); \
  lua_pushstring(l, qPrintable(theme.FIELD)); \
  lua_settable(l, -3);
#define ADD_FIELD(FIELD) ADD_FIELD_AUX(#FIELD, FIELD)
  
  ADD_FIELD(name);
  ADD_FIELD(description);
  ADD_FIELD_AUX("desktop_file", desktopFile);
  ADD_FIELD(file_name);
  lua_pushstring(l, "variants");
  lua_newtable(l);
  for (int i = 0; i < theme.variants.size(); i++) {
    lua_pushnumber(l, i);
    lua_pushstring(l, qPrintable(theme.variants[i]));
    lua_settable(l, -3);
  }
  lua_settable(l, -3);
  
#undef ADD_FIELD_AUX
#undef ADD_FIELD
  
  lua_pushstring(l, "description");
  lua_pushstring(l, qPrintable(theme.description));
  lua_settable(l, -3);
  
  lua_setglobal(l, "theme");

  // add import
  lua_pushcfunction(l, import_func);
  lua_setglobal(l, "import");
}

void Loader::initialize(::Loader::Context *ctx) {
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
  Wrapper<QFont>::register_class(l);
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
}

Loader::~Loader() {
  lua_close(m_state);
}

bool Loader::runFile(const QString& file, bool setdir) {

  QString path = QDir::cleanPath(
                    QDir::isAbsolutePath(file) ? file : m_curr_dir.filePath(file) );
  if (!QFile::exists(path)) {
    // find it in the scripts dir
    path = KStandardDirs::locate("appdata", "scripts/" + file);
  }
  if (!QFile::exists(path)) {
    // give up
    return false;
  }
  
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
  const LuaValueMap* args;
  bool allow_nil;
  T out;
  create_value_data(const QString& _key, int _size, const LuaValueMap* _args, bool _allow_nil)
    : key(_key)
    , size(_size)
    , args(_args)
    , allow_nil(_allow_nil) {
  }
};

template<typename T>
T Loader::getValue(const QString& key, int size, const LuaValueMap* args, bool allow_nil) {
  StackCheck s(m_state);

  create_value_data<T> data(key, size, args, allow_nil);
  if(lua_cpcall(m_state, create_value_func<T>, &data) != 0) {
    m_error = true;
    m_error_string = QString(lua_tostring(m_state, -1))+"\nsearched key was: "+key;
    lua_pop(m_state, 1);
    return T();
  }
  return data.out;
}

template ::Loader::Glyph Loader::getValue< ::Loader::Glyph>(const QString&, int, const LuaValueMap*,  bool);
template OptList Loader::getValue<OptList>(const QString&, int, const LuaValueMap*,  bool);
template QString Loader::getValue<QString>(const QString&, int, const LuaValueMap*,  bool);
template QStringList Loader::getValue<QStringList>(const QString&, int, const LuaValueMap*,  bool);
template QImage Loader::getValue<QImage>(const QString&, int, const LuaValueMap*,  bool);
template ImageOrMap Loader::getValue<ImageOrMap>(const QString&, int, const LuaValueMap*,  bool);
template double Loader::getValue<double>(const QString&, int, const LuaValueMap*,  bool);
template QPointF Loader::getValue<QPointF>(const QString&, int, const LuaValueMap*,  bool);
template QRectF Loader::getValue<QRectF>(const QString&, int, const LuaValueMap*,  bool);
template QColor Loader::getValue<QColor>(const QString&, int, const LuaValueMap*,  bool);
template QBrush Loader::getValue<QBrush>(const QString&, int, const LuaValueMap*,  bool);
template QFont  Loader::getValue<QFont>(const QString&, int, const LuaValueMap*,  bool);
template LuaValueMap Loader::getValue<LuaValueMap>(const QString&, int, const LuaValueMap*,  bool);

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
void Loader::retrieve<QColor>(create_value_data<QColor>* d, lua_State *l, int pos) {
  d->out = Wrapper<QColor>::get(l, pos);
}

template<>
void Loader::retrieve<QBrush>(create_value_data<QBrush>* d, lua_State *l, int pos) {
  d->out = Wrapper<QBrush>::get(l, pos);
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
void Loader::retrieve<LuaValueMap>(create_value_data<LuaValueMap>* d, lua_State *l, int pos) {
  if(lua_istable(l, pos)) {
    lua_pushnil(l);
    while (lua_next(l, pos<0 ? pos-1 : pos) != 0) {
      QString key = lua_tostring(l, -2);
      if(QPointF *res = Wrapper<QPointF>::retrieve(l, -1, Check))
        d->out[key] = *res;
      else if(QRectF *res = Wrapper<QRectF>::retrieve(l, -1, Check))
        d->out[key] = *res;
      else
        d->out[key] = lua_tonumber(l, -1);
      lua_pop(l, 1);
    }
  }
  else
    luaL_error(l, "Can't convert to a LuaValueMap (not table)");
}

template<>
void Loader::retrieve<QImage>(create_value_data<QImage>* d, lua_State *l, int pos) {
  ::Loader::Image *retv = Wrapper< ::Loader::Image>::retrieve(l, pos, AssertOk);
  d->out = retv->image();
}

template<>
void Loader::retrieve<ImageOrMap>(create_value_data<ImageOrMap>* d, lua_State *l, int pos) {
  if(::Loader::Image *img = Wrapper< ::Loader::Image>::retrieve(l, pos))
     d->out = img->image();
  else if(lua_istable(l, pos)) {

    //collect the images in this way to avoid leaking memory if Wrapper::retrieve raises an exception
    d->out = ImageMap();
    ImageMap& out = boost::get<ImageMap>(d->out);

    lua_pushnil(l);
    while (lua_next(l, pos<0 ? pos-1 : pos) != 0) {
      QRectF *rect = Wrapper<QRectF>::retrieve(l, -2, AssertOk);
      ::Loader::Image *img = Wrapper< ::Loader::Image>::retrieve(l, -1, AssertOk);

      QRect r = rect->toRect();
      out[rect->toRect()] = img->image();

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
  if(lua_isnil(l, -1)) {
    if(!data->allow_nil)
      luaL_error(l, "No such entry: %s", data->key.toAscii().constData());
  }
  else {
    if(lua_isfunction(l, -1)) {
      int nparams = 0;
      if(data->size) {
        lua_pushnumber(l, data->size);
        nparams++;
      }
      if(data->args) {
        lua_pushvaluemap(l, data->args);
        nparams++;
      }
      lua_call(l, nparams, 1);
    }

    retrieve<T>(data, l, -1);
  }

  lua_pop(l, 1);
  return 0;
}


int Loader::import_func(lua_State *l) {
  lua_getfield(l, LUA_REGISTRYINDEX, API_LOADER);
  Loader* api = reinterpret_cast<Loader*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  int n = lua_gettop(l);
  if (n != 1)
    luaL_error(l, "Wrong argument count for \"import\"");
  QString file(lua_tostring(l, 1));
  lua_pop(l, n);

  if (!api->runFile(file, false)) {
    luaL_error(l, "Error importing \"%s\":\n%s", file.toAscii().constData(),
                                          api->errorString().toAscii().constData() );
  }
  
  return 0;
}

int Loader::read_desktop_file(lua_State* l) {
  lua_getfield(l, LUA_REGISTRYINDEX, API_LOADER);
  Loader* api = reinterpret_cast<Loader*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  int n = lua_gettop(l);
  if (n != 0)
    luaL_error(l, "Wrong argument count for \"desktop_file\"");
  lua_pop(l, n);
  
  QString filePath = api->currDir().filePath("theme.desktop");
  if (!KDesktopFile::isDesktopFile(filePath)) {
    // no desktop file
    return 0;
  }
  
  KDesktopFile theme(filePath);
  
  // set theme fields according to the desktop file
  lua_getglobal(l, "theme");
  
  QString name = theme.readName();
  if (!name.isEmpty()) {
    lua_pushstring(l, "name");
    lua_pushstring(l, qPrintable(name));
    lua_settable(l, -3);
  }
  
  QString description = theme.readComment();
  if (!name.isEmpty()) {
    description.replace("|", "\n");
    lua_pushstring(l, "description");
    lua_pushstring(l, qPrintable(description));
    lua_settable(l, -3);
  }
  
  KConfigGroup themeGroup = theme.desktopGroup();
  QString variants = themeGroup.readEntry("X-Tagua-Variants");
  if (!variants.isEmpty()) {
    QStringList varlist = variants.split(QRegExp("\\s*,\\s*"));
    lua_pushstring(l, "variants");
    lua_newtable(l);
    for (int i = 0; i < varlist.size(); i++) {
      lua_pushnumber(l, i + 1);
      lua_pushstring(l, qPrintable(varlist[i]));
      lua_settable(l, -3);
    }
    lua_settable(l, -3);
  }
  
  return 0;
}

} // namespace LuaLoader
