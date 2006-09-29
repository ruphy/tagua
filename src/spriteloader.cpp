/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <map>
#include "loader/theme.h"
#include "spriteloader.h"

class SpriteLoader::Loader : public ::Loader::Theme {
public:
  int m_ref_count;
  Loader(const QString& s)
    : ::Loader::Theme(s)
    , m_ref_count(0) {
  }
};

/* inherit instead of typedef to ease forward declaration :) */
class SpriteLoader::LoadersCache : public
  std::map<QString, SpriteLoader::Loader*> {
};

SpriteLoader::LoadersCache SpriteLoader::loaders;

SpriteLoader::SpriteLoader()
: m_loader(NULL)
, m_size(0)
{
}

SpriteLoader::~SpriteLoader() {
  flush();
}

void SpriteLoader::flush() {
  if(m_loader) {
    /* unref the size */
    if(m_size)
      m_loader->unrefSize(m_size);

    /* unref the loader, and possibly destroy it */
    if(!--m_loader->m_ref_count) {
      delete m_loader;
      loaders.erase(m_base);
    }
    m_loader = NULL;
  }
}

void SpriteLoader::setBasePath(const QString& base) {
  //QString base = PixmapLoader::Loader::resolveBasePath(__base);

  if(base == m_base)
    return;

  flush();
  m_base = base;
}

void SpriteLoader::setSize(int s) {
  if(m_loader) {
    if(s)
      m_loader->refSize(s);
    if(m_size)
      m_loader->unrefSize(m_size);
  }
  m_size = s;
}

void SpriteLoader::initialize() {
  if(m_loader)
    return;

  /* try to get a loader */
  if(loaders.count(m_base))
    m_loader = loaders[m_base];
  else {
    m_loader = new Loader(m_base);
    loaders[m_base] = m_loader;
  }

  m_loader->m_ref_count++;
  m_loader->refSize(m_size);
}

QPixmap SpriteLoader::operator()(const QString& id) {
  if(!m_size || m_base.isEmpty())
    return QPixmap();

  if(!m_loader)
    initialize();

  return m_loader->getPixmap(id, m_size);
}

