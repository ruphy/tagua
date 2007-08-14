#include "actioncollection.h"

#include <QAction>

ActionCollection::ActionCollection() { }

void ActionCollection::add(QAction* action) {
  m_actions.append(action);
}

void ActionCollection::add(QAction* action, const Callback& callback) {
  add(action);
  m_callbacks.insert(action, callback);
  connect(action, SIGNAL(triggered()), this, SLOT(actionTriggered()));
}

QList<QAction*> ActionCollection::actions() const {
  return m_actions;
}

void ActionCollection::actionTriggered() {
  QAction* action = qobject_cast<QAction*>(sender());
  
  if (action && m_callbacks.contains(action)) {
    m_callbacks.value(action)();
  }
}


