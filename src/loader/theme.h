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

typedef std::map<QRect, QPixmap, ::LuaApi::RectLess> PixmapMap;
typedef boost::variant<QPixmap, PixmapMap> PixmapOrMap;

/**
  * @class Theme <loader/theme.h>
  * @brief A class that represents and caches all images loaded by a theme.
  *
  * This class will be created once for each lua-scripted theme, and will cache
  * images loaded from that theme.
  */
class Theme : public QObject {
Q_OBJECT

private:
  QString m_file;

  /** there will be one such class for each size, and i will store pixmaps and glyphs */
  class SizeCache {
  public:
    typedef std::map<QString, PixmapOrMap> PixmapsCache;
    typedef std::map<QString, Glyph> GlyphsCache;
    int m_ref_count;
    PixmapsCache m_pixmaps_cache;
    GlyphsCache m_glyphs_cache;

    SizeCache()
      : m_ref_count(0) {}
  };

  typedef std::map<int, SizeCache> Cache;

  Context m_context;
  LuaApi::Loader m_lua_loader;
  Cache m_cache;

  static PixmapOrMap to_pixmap_map(const ::LuaApi::ImageOrMap& m);

private slots:
  void onSettingsChanged();

public:
  /** Constructor, created the class from a lua theme file */
  Theme(const QString& lua_file);
  ~Theme();

  /** References the size \a size, enabling you to get pixmaps of size \a size */
  void refSize(int size);

  /** Unreferences the size */
  void unrefSize(int size);

  /** Loads a pixmap */
  QPixmap getPixmap(const QString& key, int size);

  /** Loads a pixmap -or- map of rects->pixmaps */
  PixmapOrMap getPixmapMap(const QString& key, int size);

  /** Loads a glyph */
  Glyph getGlyph(const QString& key, int size);
};

} //end namespace loader

#endif //LOADER__THEME_H
