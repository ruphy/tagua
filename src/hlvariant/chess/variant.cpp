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


