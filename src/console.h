/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CONSOLE_H
#define CONSOLE_H

#include <QWidget>
#include <QRegExp>
#include <boost/shared_ptr.hpp>
#include "histlineedit.h"
#include "console_p.h"

class QLabel;
class QTextEdit;
class TextNotifier;

class Console : public QWidget {
Q_OBJECT
  HistLineEdit* edit;
  QTextEdit* display;
  QLabel* prompt;
  static QRegExp promptRegexp;
  boost::shared_ptr<TextNotifier> m_notifier;
  LuaConsoleHighlighter m_highlighter;
public:
  Console(QWidget* parent, const QString& caption);
  inline QLineEdit* commandLine() const { return edit; }
  void clear();
  void setNotifier(const boost::shared_ptr<TextNotifier>& notifier) { m_notifier = notifier; }
public Q_SLOTS:
  void displayText(QString, int offset);
  void echo(const QString&);
protected Q_SLOTS:
  void input(const QString&);
Q_SIGNALS:
  void receivedInput(const QString&);
  void notify();
};

class TextNotifier {
public:
  virtual ~TextNotifier() { }
  virtual void textNotify(const QString& text) = 0;
};

#endif // CONSOLE_H
