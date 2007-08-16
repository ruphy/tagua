/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "tabwidget.h"

#include <QPushButton>
#include <QTabBar>

#include <KIcon>
#include <KLocalizedString>

#include <iostream>

TabWidget::TabWidget(QWidget* parent)
: KTabWidget(parent) {
  m_close = new QPushButton(this);
  m_close->setToolTip(i18n("Close the current tab"));
  m_close->setIcon(KIcon("tab-remove"));
  m_close->adjustSize();
  
  connect(m_close, SIGNAL(clicked()), this, SIGNAL(closeTab()));
  setCornerWidget(m_close);
  
  setTabBarHidden(true);
}

// void TabWidget::insertTab(int index, QWidget* widget, const QString& caption) {
//   KTabWidget::insertTab(index, widget, caption);
//   std::cout << "showing" << std::endl;
//   if (count() > 1)
//     setTabBarHidden(false);
// }
// 
// void TabWidget::removeTab(int index) {
//   KTabWidget::removeTab(index);
//   std::cout << "hiding" << std::endl;
//   if (count() <= 1)
//     setTabBarHidden(true);
// }
