/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variant.h"
#include "../tagua_wrapped.h"

namespace HLVariant {
namespace Dummy {

const char* Variant::m_name = "Dummy";
const char* Variant::m_theme_proxy = "Chess";

void Variant::setupMove(NormalUserMove& m) const {
  m.promotionType = m_actions.promotion();
}

ActionCollection* Variant::actions() {
  return &m_actions;
}

} // namespace Dummy
} // namespace HLVariant
