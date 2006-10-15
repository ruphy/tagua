/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include <iostream>
#include <QHash>
#include "common.h"
#include "global.h"
#include "loader/theme.h"

namespace Loader {

PixmapOrMap Theme::to_pixmap_map(const ::LuaApi::ImageOrMap& m) {
  if(const QImage *i = boost::get<QImage>(&m)) {
    return PixmapOrMap(QPixmap::fromImage(*i));
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

Theme::Theme(const QString& lua_file)
: m_file(lua_file)
, m_context()
, m_lua_loader(&m_context) {

  m_lua_loader.runFile(m_file.toAscii().constData());
  if(m_lua_loader.error())
    std::cout << "SCRIPT LOAD ERROR:" << std::endl << m_lua_loader.errorString() << std::endl;
  settings.onChange(this, SLOT(onSettingsChanged()));
  onSettingsChanged();
}

Theme::~Theme() {
  if(!m_cache.empty()) {
    std::cout << " --> Error in Theme::~Theme, sizes still referenced" << std::endl;
  }
}

void Theme::onSettingsChanged() {
  SettingMap<QString> s_lua = settings.group("lua-settings").map<QString>("entry", "file-name");
  Settings entry = s_lua.insert(m_file);
  OptList ol = m_lua_loader.getOptList("options");
  if(options_list_load_from_settings(ol, entry.group("options"))) {
    for(Cache::iterator it = m_cache.begin(); it != m_cache.end(); ++it)
      it->second.m_cache.clear();
  }
}

void Theme::refSize(int size) {
  m_cache[size].m_ref_count++;
}

void Theme::unrefSize(int size) {
  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    std::cout << " --> Error in Theme::unrefSize, size not referenced " << size << std::endl;
    return;
  }

  if(!--it->second.m_ref_count)
    m_cache.erase(size);
}

PixmapOrMap Theme::getPixmapMap(const QString& key, int size) {
  if(m_lua_loader.error())
    return PixmapOrMap();

  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    std::cout << " --> Error in Theme::getPixmap, size not referenced " << size << std::endl;
    return PixmapOrMap();
  }

  SizeCache::Cache::iterator pix = it->second.m_cache.find(key);
  if(pix != it->second.m_cache.end())
    return pix->second;

  PixmapOrMap retv = to_pixmap_map(m_lua_loader.getImageMap(key, size));
  if(m_lua_loader.error()) {
    std::cout << "SCRIPT RUN ERROR:" << std::endl << m_lua_loader.errorString() << std::endl;
    m_lua_loader.clearError();
  }

  it->second.m_cache[key] = retv;
  return retv;
}

QPixmap Theme::getPixmap(const QString& key, int size) {
  PixmapOrMap p = getPixmapMap(key, size);
  if(QPixmap *px = boost::get<QPixmap>(&p))
    return *px;
  return QPixmap();
}

Glyph Theme::getGlyph(const QString& key, int size) {
  if(m_lua_loader.error())
    return Glyph();

  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    std::cout << " --> Error in Theme::getGlyph, size not referenced " << size << std::endl;
    return Glyph();
  }

  SizeCache::Cache2::iterator pix = it->second.m_cache2.find(key);
  if(pix != it->second.m_cache2.end())
    return pix->second;

  Glyph retv = m_lua_loader.getGlyph(key);
  retv.m_font.setPointSize(size+retv.m_delta);

  if(m_lua_loader.error()) {
    std::cout << "SCRIPT RUN ERROR:" << std::endl << m_lua_loader.errorString() << std::endl;
    m_lua_loader.clearError();
  }

  it->second.m_cache2[key] = retv;
  return retv;
}

#include "theme.moc"

} //end namespace Loader
