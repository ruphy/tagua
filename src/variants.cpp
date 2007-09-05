/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variants.h"
#include "hlvariant/tagua_wrapped.h"
#include "hlvariant/chess/variant.h"
#include "hlvariant/crazyhouse/variant.h"
#include "hlvariant/dummy/variant.h"
#include "hlvariant/minichess5/variant.h"
#include "hlvariant/shogi/variant.h"
#include "hlvariant/minishogi/variant.h"

using namespace HLVariant;

template <typename Variant>
void register_variant(Variants* variants) {
  variants->addFactory(
    Variant::m_name,
    new WrappedVariantFactory<Variant>);
}


Variants::Variants() {
  register_variant<Chess::Variant>(this);
  register_variant<Minichess5::Variant>(this);
  register_variant<Crazyhouse::Variant>(this);
  register_variant<Dummy::Variant>(this);
  register_variant<Shogi::Variant>(this);
  register_variant<MiniShogi::Variant>(this);
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

void Variants::addFactory(const QString& name, VariantFactory* factory) {
  m_factories[name] = factory;
  std::cout << "added factory for variant " << name << std::endl;
}

QStringList Variants::all() const {
  QStringList s;
  for (Factories::const_iterator end = m_factories.end(), it = m_factories.begin(); it != end; ++it) {
    s << it->first;
  }
  
  return s;
}

