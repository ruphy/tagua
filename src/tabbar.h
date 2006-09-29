/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef TABBAR_H
#define TABBAR_H

#include <QTabBar>
#include <QTabWidget>
class QToolButton;

class TabBar : public QTabBar {
Q_OBJECT
public:
  TabBar(QWidget* parent = 0);
};

class TabWidget : public QTabWidget {
Q_OBJECT
  QToolButton* m_close;
public:
  TabWidget(QWidget* parent = 0);
  
  void showTabBar();
  void hideTabBar();
signals:
  void closeTab();
};

#endif // TABBAR_H

