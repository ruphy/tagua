#include "variant.h"
#include "../tagua_wrapped.h"
#include "actioncollection.h"

#include <QActionGroup>
#include <KAction>

namespace HLVariant {
namespace Chess {

const char* Variant::m_name = "Chess";
const char* Variant::m_theme_proxy = "Chess";

QAction* createAction(QActionGroup* group, ActionCollection* actions, 
                      const KIcon& icon, const QString& text) {
  QAction* action = new KAction(icon, text, group);
  action->setCheckable(true);
  group->addAction(action);
  actions->add(action);
  return action;
}

void Variant::setupActions(ActionCollection* actions) {
  QActionGroup* group = new QActionGroup(actions);
  
  QAction* queen = createAction(group, actions, KIcon("promoteQueen"), "Promote to queen");
  createAction(group, actions, KIcon("promoteRook"), "Promote to rook");
  createAction(group, actions, KIcon("promoteBishop"), "Promote to bishop");
  createAction(group, actions, KIcon("promoteKnight"), "Promote to knight");
  
  queen->setChecked(true);
}

} // namespace Chess
} // namespace HLVariant


