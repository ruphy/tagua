/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variant.h"

#include <functional>
#include <boost/bind.hpp>

#include <QActionGroup>
#include <KAction>

#include "../tagua_wrapped.h"
#include "actioncollection.h"

namespace HLVariant {
namespace Chess {

const char* Variant::m_name = "Chess";
const char* Variant::m_theme_proxy = "Chess";

void Variant::setupMove(NormalUserMove& move) const {
  move.promotionType = m_actions.promotion();
}

ActionCollection* Variant::actions() {
  return &m_actions;
}

DEFINE_VARIANT_FACTORY();

} // namespace Chess
} // namespace HLVariant


