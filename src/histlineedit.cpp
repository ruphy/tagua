/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QKeyEvent>
#include "histlineedit.h"

HistLineEdit::HistLineEdit(QWidget* parent)
: QLineEdit(parent) {
  current = 0;

  connect(this, SIGNAL(returnPressed()), this, SLOT(updateHistory()));
 }

void HistLineEdit::keyPressEvent(QKeyEvent* e) {
  if (echoMode() == Password) {
    e->ignore();
    QLineEdit::keyPressEvent(e);
    return;
  }

  if (e->key() == Qt::Key_Up) {
    if (current > 0)
      setHistoryText(current-1);
  }
  else if (e->key() == Qt::Key_Down) {
    if (current < history.size())
      setHistoryText(current+1);
  }
  else if (e->key() == Qt::Key_PageUp)
    emit pageUp();
  else if (e->key() == Qt::Key_PageDown)
    emit pageDown();
  else e->ignore();

  QLineEdit::keyPressEvent(e);
}

void HistLineEdit::setHistoryText(uint index) {
  if (current == history.size())
    buffer = text();

  if (index == history.size())
    setText(buffer);
  else
    setText(history[index]);

  current = index;
}

void HistLineEdit::updateHistory() {
  if (echoMode() != Password) {

    history.push_back(text());
    buffer = "";
    current = history.size();
  }

  emit receivedInput(text());
  setText("");
}
