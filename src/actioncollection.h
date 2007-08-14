#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <boost/function.hpp>
#include <QList>
#include <QHash>
#include <QObject>

class QAction;

class ActionCollection : public QObject {
Q_OBJECT
  QList<QAction*> m_actions;
public:
  ActionCollection();
  
  void add(QAction* action);
  QList<QAction*> actions() const;
};

#endif // ACTIONCOLLECTION_H

