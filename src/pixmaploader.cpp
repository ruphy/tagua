/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <map>
#include <iostream>
#include "common.h"
#include "loader/theme.h"
#include "pixmaploader.h"
#include "themeinfo.h"

class PixmapLoader::ThemeLoader : public Loader::Theme {
public:
  int m_ref_count;

  ThemeLoader(const ThemeInfo& t)
    : Loader::Theme(t)
    , m_ref_count(0) {
  }
};


PixmapLoader::ThemeLoadersCache PixmapLoader::s_loaders;


PixmapLoader::PixmapLoader()
: m_loader(NULL)
, m_size(0)
{
}

PixmapLoader::~PixmapLoader() {
  flush();
}

void PixmapLoader::flush() {
  if (m_loader) {
    /* unref the size */
    if(m_size)
      m_loader->unrefSize(m_size);
    m_loader->unrefSize(0);

    /* unref the loader, and possibly destroy it */
    if(!--m_loader->m_ref_count) {
      delete m_loader;
      s_loaders.remove(m_theme);
    }
    m_loader = NULL;
  }
}

void PixmapLoader::setTheme(const ThemeInfo& theme) {
  if (theme == m_theme)
    return;

  flush();
  m_theme = theme;
}

void PixmapLoader::setSize(int s) {
  if(s == m_size)
    return;

  if(m_loader) {
    if(s)
      m_loader->refSize(s);
    if(m_size)
      m_loader->unrefSize(m_size);
  }
  m_size = s;
}

void PixmapLoader::initialize() {
  if (m_loader)
    return;

  /* try to get a loader */
  m_loader = s_loaders.value(m_theme, 0);
  if (!m_loader) {
    // no loader, yet
    // create it
    m_loader = new ThemeLoader(m_theme);
    s_loaders.insert(m_theme, m_loader);
  }
  
  Q_ASSERT(m_loader);
  
  m_loader->m_ref_count++;
  if (m_size)
    m_loader->refSize(m_size);

  m_loader->refSize(0);
}

QPixmap PixmapLoader::operator()(const QString& id) {
  return getValue<QPixmap>(id);
}

template<typename T>
T PixmapLoader::getValue(const QString& id, const ::LuaApi::LuaValueMap* args) {
  if (!m_size || !m_theme)
    return T();

  if (!m_loader)
    initialize();

  return m_loader->getValue<T>(id, m_size, args);
}

template QPixmap PixmapLoader::getValue<QPixmap>(const QString&, const ::LuaApi::LuaValueMap*);
template Loader::PixmapOrMap PixmapLoader::getValue<Loader::PixmapOrMap>(const QString&, const ::LuaApi::LuaValueMap*);
template Loader::Glyph       PixmapLoader::getValue<Loader::Glyph>(const QString&, const ::LuaApi::LuaValueMap*);
template double  PixmapLoader::getValue<double>(const QString&, const ::LuaApi::LuaValueMap*);
template QPointF PixmapLoader::getValue<QPointF>(const QString&, const ::LuaApi::LuaValueMap*);
template QRectF  PixmapLoader::getValue<QRectF>(const QString&, const ::LuaApi::LuaValueMap*);
template QBrush  PixmapLoader::getValue<QBrush>(const QString&, const ::LuaApi::LuaValueMap*);
template QColor  PixmapLoader::getValue<QColor>(const QString&, const ::LuaApi::LuaValueMap*);
template QFont   PixmapLoader::getValue<QFont>(const QString&, const ::LuaApi::LuaValueMap*);
template ::LuaApi::LuaValueMap  PixmapLoader::getValue< ::LuaApi::LuaValueMap>(const QString&, const ::LuaApi::LuaValueMap*);


template<typename T>
T PixmapLoader::getStaticValue(const QString& id, const ::LuaApi::LuaValueMap* args) {
  if (!m_theme)
    return T();

  if (!m_loader)
    initialize();

  return m_loader->getValue<T>(id, 0, args);
}

template QPixmap PixmapLoader::getStaticValue<QPixmap>(const QString&, const ::LuaApi::LuaValueMap*);
template Loader::PixmapOrMap PixmapLoader::getStaticValue<Loader::PixmapOrMap>(const QString&, const ::LuaApi::LuaValueMap*);
template Loader::Glyph       PixmapLoader::getStaticValue<Loader::Glyph>(const QString&, const ::LuaApi::LuaValueMap*);
template double  PixmapLoader::getStaticValue<double>(const QString&, const ::LuaApi::LuaValueMap*);
template QPointF PixmapLoader::getStaticValue<QPointF>(const QString&, const ::LuaApi::LuaValueMap*);
template QRectF  PixmapLoader::getStaticValue<QRectF>(const QString&, const ::LuaApi::LuaValueMap*);
template QBrush  PixmapLoader::getStaticValue<QBrush>(const QString&, const ::LuaApi::LuaValueMap*);
template QColor  PixmapLoader::getStaticValue<QColor>(const QString&, const ::LuaApi::LuaValueMap*);
template QFont  PixmapLoader::getStaticValue<QFont>(const QString&, const ::LuaApi::LuaValueMap*);
template ::LuaApi::LuaValueMap  PixmapLoader::getStaticValue< ::LuaApi::LuaValueMap>(const QString&, const ::LuaApi::LuaValueMap*);
