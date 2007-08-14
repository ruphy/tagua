#include "variant.h"
#include "../tagua_wrapped.h"
#include "actioncollection.h"

#include <KAction>

namespace HLVariant {
namespace Chess {

const char* Variant::m_name = "Chess";
const char* Variant::m_theme_proxy = "Chess";

void Variant::setupActions(ActionCollection* actions) {
  actions->add(new KAction(KIcon("promoteQueen"), "Promote to queen", actions));
  actions->add(new KAction(KIcon("promoteRook"), "Promote to rook", actions));
  actions->add(new KAction(KIcon("promoteBishop"), "Promote to bishop", actions));
  actions->add(new KAction(KIcon("promoteKnight"), "Promote to knight", actions));
}

} // namespace Chess
} // namespace HLVariant


