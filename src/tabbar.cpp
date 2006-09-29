/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "tabbar.h"
#include "settings.h"
#include <QStylePainter>
#include <QToolButton>
#include <QDir>
#include <QLayout>

TabBar::TabBar(QWidget* parent)
: QTabBar(parent) {
}


TabWidget::TabWidget(QWidget* parent)
: QTabWidget(parent) {
  setTabBar(new TabBar);

  m_close = new QToolButton(this);
  connect(m_close, SIGNAL(clicked()), this, SIGNAL(closeTab()));

  settings.qSettings()->beginGroup("MainWindow");
  QString iconDir = data_dir() + "/icons/";
  settings.qSettings()->endGroup();

  m_close->setIcon(QIcon(QDir(iconDir).filePath("close_tab.png")));
  m_close->adjustSize();
  setCornerWidget(m_close, Qt::TopRightCorner);

  tabBar()->hide();
}

void TabWidget::showTabBar() {
  tabBar()->show();
}

void TabWidget::hideTabBar() {
  tabBar()->hide();
}


#include "tabbar.moc"
