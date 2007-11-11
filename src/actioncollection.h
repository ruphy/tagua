/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

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

