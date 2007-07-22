/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HISTLINEEDIT_H
#define HISTLINEEDIT_H

#include <vector>
#include <QLineEdit>

class HistLineEdit : public QLineEdit {
  Q_OBJECT
  std::vector<QString> history;
  uint current;

  void setHistoryText(uint index);
  QString buffer;
public:
  HistLineEdit(QWidget* parent);
protected:
  void keyPressEvent(QKeyEvent*);
public Q_SLOTS:
  void updateHistory();
Q_SIGNALS:
  void pageUp();
  void pageDown();
  void receivedInput(const QString&);
};

#endif // HISTLINEEDIT_H
