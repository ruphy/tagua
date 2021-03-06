/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include "theme.h"
#include <QHash>
#include <KDebug>
#include "common.h"
#include "mastersettings.h"

namespace Loader {

PixmapOrMap Theme::to_pixmap_map(const ::LuaApi::ImageOrMap& m) {
  if(const QImage *i = boost::get<QImage>(&m)) {
    return i->isNull() ? PixmapOrMap() : PixmapOrMap(QPixmap::fromImage(*i));
  }
  else if(const ::LuaApi::ImageMap *i = boost::get< ::LuaApi::ImageMap>(&m)) {
    PixmapMap p;
    for(::LuaApi::ImageMap::const_iterator it = i->begin(); it != i->end(); ++it) {
      p[it->first] = QPixmap::fromImage(it->second);
    }
    return PixmapOrMap(p);
  }
  else
    return PixmapOrMap();
}

Theme::Theme(const ThemeInfo& theme)
: m_theme(theme)
, m_context()
, m_lua_loader(&m_context, theme) {
  m_lua_loader.runFile(m_theme.file_name);
  if(m_lua_loader.error())
    kError() << "Script load error:" << m_lua_loader.errorString();

  settings().onChange(this, "onSettingsChanged");
  onSettingsChanged();
}

Theme::~Theme() {
  if(!m_cache.empty())
    kError() << "Sizes still referenced.";
}

void Theme::onSettingsChanged() {
  SettingMap<QString> s_lua = settings().group("lua-settings").map<QString>("entry", "file-name");
  Settings entry = s_lua.insert(m_theme.file_name);
  OptList ol = m_lua_loader.getValue<OptList>("options", 0, NULL, true);
  if(m_lua_loader.error()) {
    m_lua_loader.clearError();
    return;
  }

  if(options_list_load_from_settings(ol, entry.group("options")))
  for(Cache::iterator it = m_cache.begin(); it != m_cache.end(); ++it)
    it->second.m_pixmaps_cache.clear();
}

void Theme::refSize(int size) {
  m_cache[size].m_ref_count++;
}

void Theme::unrefSize(int size) {
  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    kError() << "Size" << size << "not referenced.";
    return;
  }

  if(!--it->second.m_ref_count)
    m_cache.erase(size);
}

template<typename T>
T Theme::getValue(const QString& key, int size, const ::LuaApi::LuaValueMap* args, bool allow_nil) {
  if(m_lua_loader.error())
    return T();

  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    kError() << "Size" << size << "not referenced.";
    return T();
  }

  T retv = m_lua_loader.getValue<T>(key, size, args, allow_nil);

  if(m_lua_loader.error()) {
    kError() << "Script run error:" << m_lua_loader.errorString();
    m_lua_loader.clearError();
  }

  return retv;
}

template<>
PixmapOrMap Theme::getValue<PixmapOrMap>(const QString& key, int size, const ::LuaApi::LuaValueMap* args, bool allow_nil) {
  if(m_lua_loader.error())
    return PixmapOrMap();

  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    kError() << "Size" << size << "not referenced.";
    return PixmapOrMap();
  }

  if(!args) {
    SizeCache::PixmapsCache::iterator pix = it->second.m_pixmaps_cache.find(key);
    if(pix != it->second.m_pixmaps_cache.end())
      return pix->second;
  }

  PixmapOrMap retv = to_pixmap_map(m_lua_loader.getValue< ::LuaApi::ImageOrMap>(key, size, args, allow_nil));
  if(m_lua_loader.error()) {
    kError() << "Script run error:" << m_lua_loader.errorString();
    m_lua_loader.clearError();
  }

  if(!args)
    it->second.m_pixmaps_cache[key] = retv;
  return retv;
}

template<>
QPixmap Theme::getValue<QPixmap>(const QString& key, int size, const ::LuaApi::LuaValueMap* args, bool allow_nil) {
  PixmapOrMap p = getValue<PixmapOrMap>(key, size, args, allow_nil);
  if(QPixmap *px = boost::get<QPixmap>(&p))
    return *px;
  return QPixmap();
}

template<>
Glyph Theme::getValue<Glyph>(const QString& key, int size, const ::LuaApi::LuaValueMap* args, bool allow_nil) {
  if(m_lua_loader.error())
    return Glyph();

  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    kError() << "Size" << size << "not referenced.";
    return Glyph();
  }

  if(!args) {
    SizeCache::GlyphsCache::iterator pix = it->second.m_glyphs_cache.find(key);
    if(pix != it->second.m_glyphs_cache.end())
      return pix->second;
  }

  Glyph retv = m_lua_loader.getValue<Glyph>(key, size, args, allow_nil);

  if(m_lua_loader.error()) {
    kError() << "Script run error:" << m_lua_loader.errorString();
    m_lua_loader.clearError();
  }

  retv.m_font.setPointSize(size+retv.delta());
  if(!args)
    it->second.m_glyphs_cache[key] = retv;
  return retv;
}

template double Theme::getValue<double>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);
template QPointF Theme::getValue<QPointF>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);
template QRectF Theme::getValue<QRectF>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);
template QBrush Theme::getValue<QBrush>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);
template QColor Theme::getValue<QColor>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);
template QFont Theme::getValue<QFont>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);
template ::LuaApi::LuaValueMap Theme::getValue< ::LuaApi::LuaValueMap>(const QString&, int, const ::LuaApi::LuaValueMap* args, bool allow_nil);

} //end namespace Loader
