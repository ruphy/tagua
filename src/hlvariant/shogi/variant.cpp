/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variant.h"
#include "../tagua_wrapped.h"

namespace HLVariant {
namespace Shogi {

const char* Variant::m_name = "Shogi";
const char* Variant::m_theme_proxy = "Shogi";

VariantInfo* Variant::static_variant = 0;

VariantInfo* Variant::info() {
  if (!static_variant)
    static_variant = new WrappedVariantInfo<Variant>;
  return static_variant;
}

} // namespace Shogi
} // namespace HLVariant
