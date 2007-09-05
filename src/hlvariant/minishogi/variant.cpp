/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
	    (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variant.h"
#include "../tagua_wrapped.h"

namespace HLVariant {
namespace MiniShogi {

const char* Variant::m_name = "MiniShogi";
const char* Variant::m_theme_proxy = "Shogi";

void Variant::setupMove(NormalUserMove& m) const {
  m.promotionType = m_actions.promotion() ? 0 : -1;
}

ActionCollection* Variant::actions() {
  return &m_actions;
}

DEFINE_VARIANT_FACTORY();

} // namespace MiniShogi
} // namespace HLVariant
