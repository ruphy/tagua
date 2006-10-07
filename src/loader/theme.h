/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LOADER__THEME_H
#define LOADER__THEME_H

#include <map>
#include <QPixmap>
#include <QObject>
#include "loader/context.h"
#include "luaapi/loader.h"

namespace Loader {

class Theme : public QObject {
Q_OBJECT
  QString m_file;
public:
  class SizeCache {
  public:
    typedef std::map<QString, QPixmap> Cache;
    int m_ref_count;
    Cache m_cache;

    SizeCache()
      : m_ref_count(0) {}
  };
private:
  typedef std::map<int, SizeCache> Cache;

  Context m_context;
  LuaApi::Loader m_lua_loader;
  Cache m_cache;

public:
  Theme(const QString& lua_file);
  ~Theme();

  void refSize(int size);
  void unrefSize(int size);

  QPixmap getPixmap(const QString& key, int size);
private slots:
  void onSettingsChanged();
};

} //end namespace loader

#endif //LOADER__THEME_H