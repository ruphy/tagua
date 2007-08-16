/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <KTabWidget>

class QPushButton;

class TabWidget : public KTabWidget {
Q_OBJECT
  QPushButton* m_close;
public:
  TabWidget(QWidget* parent);
  
//   void insertTab(int index, QWidget* widget, const QString& caption);
//   void removeTab(int index);
Q_SIGNALS:
  void closeTab();
};

#endif // TABWIDGET_H
