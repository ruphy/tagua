#include "actioncollection.h"

#include <QAction>

ActionCollection::ActionCollection() { }

void ActionCollection::add(QAction* action) {
  m_actions.append(action);
}

QList<QAction*> ActionCollection::actions() const {
  return m_actions;
}


