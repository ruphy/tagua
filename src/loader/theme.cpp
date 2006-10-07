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
  OptList ol = m_lua_loader.getOptions();
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

QPixmap Theme::getPixmap(const QString& key, int size) {
  if(m_lua_loader.error())
    return QPixmap();

  Cache::iterator it = m_cache.find(size);
  if(it == m_cache.end()) {
    std::cout << " --> Error in Theme::getPixmap, size not referenced " << size << std::endl;
    return QPixmap();
  }

  SizeCache::Cache::iterator pix = it->second.m_cache.find(key);
  if(pix != it->second.m_cache.end())
    return pix->second;

  QPixmap retv = QPixmap::fromImage(m_lua_loader.getImage(key, size));
  if(m_lua_loader.error()) {
    std::cout << "SCRIPT RUN ERROR:" << std::endl << m_lua_loader.errorString() << std::endl;
    m_lua_loader.clearError();
  }

  it->second.m_cache[key] = retv;
  return retv;
}

#include "theme.moc"

} //end namespace Loader
