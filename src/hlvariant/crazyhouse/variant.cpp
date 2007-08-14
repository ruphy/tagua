#include "variant.h"
#include "../tagua_wrapped.h"

namespace HLVariant {
namespace Crazyhouse {

const char* Variant::m_name = "Crazyhouse";
const char* Variant::m_theme_proxy = "Chess";

void Variant::setupMove(NormalUserMove& m) const {
  m.promotionType = m_actions.promotion();
}

ActionCollection* Variant::actions() {
  return &m_actions;
}

} // namespace Chess
} // namespace HLVariant


