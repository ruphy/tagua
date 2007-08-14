#ifndef ACTIONCOLLECTION_H
#define ACTIONCOLLECTION_H

#include <boost/function.hpp>
#include <QList>
#include <QHash>
#include <QObject>

class QAction;

class ActionCollection : public QObject {
Q_OBJECT
  typedef boost::function<void()> Callback;
  QList<QAction*> m_actions;
  QHash<QAction*, Callback> m_callbacks;
public:
  ActionCollection();
  
  void add(QAction* action);
  void add(QAction* action, const Callback& callback);
  QList<QAction*> actions() const;
private Q_SLOTS:
  void actionTriggered();
};

#endif // ACTIONCOLLECTION_H

