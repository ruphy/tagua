#include "variant.h"
#include "../tagua_wrapped.h"

namespace HLVariant {
namespace Chess {

const char* Variant::m_name = "Chess";
const char* Variant::m_theme_proxy = "Chess";
VariantInfo* Variant::static_chess_variant = 0;


VariantInfo* Variant::info() {
  if (!static_chess_variant)
    static_chess_variant = new WrappedVariantInfo<Variant>;
  return static_chess_variant;
}

} // namespace Chess
} // namespace HLVariant


