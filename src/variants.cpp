/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variants.h"

using namespace HLVariant;

#define DECLARE_FACTORY(VARIANT) \
  namespace HLVariant { \
  namespace VARIANT { \
    VariantFactory* createFactory(); \
  } \
  }
DECLARE_FACTORY(Chess)
DECLARE_FACTORY(Minichess5)
DECLARE_FACTORY(Crazyhouse)
DECLARE_FACTORY(Dummy)
DECLARE_FACTORY(Shogi)
DECLARE_FACTORY(MiniShogi)
#undef DECLARE_FACTORY

Variants::Variants() {
  addFactory(Chess::createFactory());
  addFactory(Minichess5::createFactory());
  addFactory(Crazyhouse::createFactory());
  addFactory(Dummy::createFactory());
  addFactory(Shogi::createFactory());
  addFactory(MiniShogi::createFactory());
}

Variants& Variants::instance() {
  static Variants inst;
  return inst;
}

VariantFactory* Variants::getFactory(const QString& name) const {
  Factories::const_iterator it = m_factories.find(name);
  if (it != m_factories.end()) {
    return it->second;
  }
  
  return 0;
}

VariantPtr Variants::get(const QString& name) const {
  VariantFactory* factory = getFactory(name);
  return factory ? VariantPtr(factory->createVariant()) : VariantPtr();
}

void Variants::addFactory(VariantFactory* factory) {
  m_factories[factory->name()] = factory;
}

QStringList Variants::all() const {
  QStringList s;
  for (Factories::const_iterator end = m_factories.end(), it = m_factories.begin(); it != end; ++it) {
    s << it->first;
  }
  
  return s;
}

