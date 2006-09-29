/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CONSOLE_P_H
#define CONSOLE_P_H

#include "highlighter.h"
#include "hline.h"
#include "luaapi/luahl.h"
#include <QTextCharFormat>
class QTextEdit;

//BEGIN ConsoleOutput ---------------------------------------

/**
  * An utility class to perform a simple output operation
  * on a QTextEdit with RAII.
  */
class ConsoleOutput {
  QTextEdit* m_edit;
  int m_old_val;
  bool m_scroll_to;
public:
  ConsoleOutput(QTextEdit* edit, bool force_scroll = false);

  virtual ~ConsoleOutput();

  virtual void operator()(const QString& text);
};

class ConsoleHighlighter {
  std::vector<Highlighter::Pattern> m_patterns;
  QTextEdit* m_display;
  QTextCharFormat m_basic_format;
public:
  ConsoleHighlighter();
  
  void setDisplay(QTextEdit* display);
  
  void operator()(const QString& text, ConsoleOutput& output);
};

class LuaConsoleHighlighter {
  QTextEdit* m_display;
  LuaApi::Api m_api;
  class Executor : public HLineExecutor {
    QTextEdit* m_display;
    ConsoleOutput& m_output;
  public:
    Executor(QTextEdit* display, ConsoleOutput& output);
    virtual ~Executor();
    virtual void writeChunk(const QString& text);
    virtual void setFormat(const QTextCharFormat& format);
  };
public:
  LuaConsoleHighlighter();
  void setDisplay(QTextEdit* display);
  void operator()(const QString& text, ConsoleOutput& output);
};


#endif // CONSOLE_P_H
