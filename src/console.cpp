/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "console.h"

#include <QApplication>
#include <QFile>
#include <QFont>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QScrollBar>
#include <QTextCursor>
#include <QTextEdit>

#include <iostream>
#include <boost/scoped_ptr.hpp>
#include "common.h"
#include "global.h"

#include "histlineedit.h"
#include "highlighter.h"


class BackInserterPatternIteratorAdaptor : public PatternListAdaptorOutputIterator {
  std::vector<Highlighter::Pattern>& m_vector;
public:
  BackInserterPatternIteratorAdaptor(std::vector<Highlighter::Pattern>& vector)
  : m_vector(vector) { }

  virtual void add(const Highlighter::Pattern& p) {
    m_vector.push_back(p);
  }
};


ConsoleOutput::ConsoleOutput(QTextEdit* edit, bool force_scroll)
: m_edit(edit) {
  m_old_val = m_edit->verticalScrollBar()->value();
  m_scroll_to = force_scroll || (m_edit->verticalScrollBar()->value()
                                  >= m_edit->verticalScrollBar()->maximum() );

  QTextCursor cursor = m_edit->textCursor();
  cursor.movePosition(QTextCursor::End);
  m_edit->setTextCursor(cursor);
}

ConsoleOutput::~ConsoleOutput() {
  if(m_scroll_to)
    m_edit->verticalScrollBar()->setValue(m_edit->verticalScrollBar()->maximum());
  else
    m_edit->verticalScrollBar()->setValue(m_old_val);
    //m_edit->ensureCursorVisible();
}

void ConsoleOutput::operator()(const QString& text) {
  m_edit->insertPlainText(text);
}

LuaConsoleHighlighter::LuaConsoleHighlighter()
: m_display(0) {
  QString luaHighlightingFile, luaHighlightingLibrary;
  Settings hl = settings.group("highlighting");
  (hl["lua"]     |= data_dir() + "/scripts/highlighting.lua") >> luaHighlightingFile;
  (hl["library"] |= data_dir() + "/scripts/hllib.lua") >> luaHighlightingLibrary;

  m_api.runFile(qPrintable(luaHighlightingLibrary));
  m_api.runFile(qPrintable(luaHighlightingFile));
}

void LuaConsoleHighlighter::setDisplay(QTextEdit* display) {
  m_display = display;
}

LuaConsoleHighlighter::Executor::Executor(QTextEdit* display, ConsoleOutput& output)
: m_display(display)
, m_output(output) { }

void LuaConsoleHighlighter::Executor::writeChunk(const QString& text) {
  m_output(text);
}

void LuaConsoleHighlighter::Executor::setFormat(const QTextCharFormat& format) {
  m_display->setCurrentCharFormat(format);
}

LuaConsoleHighlighter::Executor::~Executor() {
//   m_output("\n");
}

void LuaConsoleHighlighter::operator()(const QString& text, ConsoleOutput& output) {
  QStringList lines = text.split('\n');

  for (int i = 0; i < lines.size(); i++) {
    const QString& line = lines[i];
    boost::scoped_ptr<HLine> hline(m_api.highlight(line));
    if (hline) {
      Executor executor(m_display, output);
      hline->run(executor);
      if (i < lines.size() - 1) output("\n");
    }
  }
}

#if 0
ConsoleHighlighter::ConsoleHighlighter()
: m_display(0) {
  BackInserterPatternIteratorAdaptor out(m_patterns);
  Highlighter::load(out);
}

void ConsoleHighlighter::setDisplay(QTextEdit* display) {
  m_display = display;
  m_basic_format = m_display->currentCharFormat();
}

void ConsoleHighlighter::operator()(const QString& text, ConsoleOutput& output) {
  Q_ASSERT(m_display);

  int offset = 0;
  m_display->setCurrentCharFormat(m_basic_format);

  while (true) {
    int best = -1;
    int best_offset = -1;
    int best_length = 0;

    // try each pattern and find the closest matching one
    for (uint i = 0; i < m_patterns.size(); i++) {
      int match_offset = text.indexOf(m_patterns[i].pattern, offset);
      if (match_offset >= 0 &&
          (best_offset == -1 || match_offset < best_offset)) {
        int length = m_patterns[i].pattern.matchedLength();
        if (length > 0) {
          best = i;
          best_offset = match_offset;
          best_length = length;
        }
      }
    }

    if (best >= 0) {
      // output the portion of the text before the matching
      // substring
//       std::cout << "outputting plain: " << text.mid(offset, best_offset - offset) << std::endl;
//       std::cout << "weight = " << m_display->currentCharFormat().fontWeight() << std::endl;
      output(text.mid(offset, best_offset - offset));

      // change format
      m_display->setCurrentCharFormat(m_patterns[best].format);

      // output the matching substring
//       std::cout << "outputting formatted: " << text.mid(best_offset, best_length) << std::endl;
//       std::cout << "weight = " << m_display->currentCharFormat().fontWeight() << std::endl;
      output(text.mid(best_offset, best_length));

      // restore basic format
      m_display->setCurrentCharFormat(m_basic_format);
//       std::cout << "restored" << std::endl;
//       std::cout << "weight = " << m_display->currentCharFormat().fontWeight() << std::endl;

      // update offset
      offset = best_offset + best_length;
    }
    else break;
  }

  // output the last portion of the text
//   std::cout << "outputting plain: " << text.mid(offset) << std::endl;
  output(text.mid(offset));
//   std::cout << "weight = " << m_display->currentCharFormat().fontWeight() << std::endl;
}
#endif

QRegExp Console::promptRegexp("([a-zA-Z0-9]*% )");

Console::Console(QWidget* parent, const QString& caption)
: QWidget(parent) {
  setObjectName("console");

  resize(600,600);
  setWindowTitle(caption);

  QBoxLayout* layout = new QVBoxLayout(this);
  QHBoxLayout* promptLayout = new QHBoxLayout;
  layout->setMargin(0);
  layout->setSpacing(0);
  promptLayout->setMargin(0);
  promptLayout->setSpacing(0);

  edit = new HistLineEdit(this);
  display = new QTextEdit(this);
  prompt = new QLabel(this);

  setFocusProxy(edit);

  layout->addWidget(display);
  layout->addLayout(promptLayout);

  promptLayout->addWidget(prompt);
  promptLayout->addWidget(edit);

  display->setFocusProxy(edit);
  display->setReadOnly(true);
  ////display->setTextFormat(QTextEdit::PlainText);
  display->setAcceptRichText(false);
  display->setWordWrapMode (QTextOption::NoWrap);
  display->setUndoRedoEnabled (false);
  Settings s_console = settings.group("console");
  if (!s_console["font"]) {
    QFont f = QApplication::font();
    f.setFamily("Monospace");
    s_console["font"] = f;
  }
  display->setFont(s_console["font"].value<QFont>());

  display->setFontWeight(QFont::Normal);
  m_highlighter.setDisplay(display);

  connect(edit, SIGNAL(receivedInput(const QString&)),
          this, SLOT(input(const QString&)));
}

void Console::echo(const QString& text) {
  ConsoleOutput output(display, true);
  display->setFontWeight(QFont::Bold);
  output(text + "\n");
  display->setFontWeight(QFont::Normal);
}

void Console::input(const QString& text) {
  if (edit->echoMode() == QLineEdit::Password) {
//    edit->setEchoMode(QLineEdit::Normal);
    echo("(password sent)");
  }
  else {
    // echo inputted text
    echo(text);
  }

  if (m_notifier)
    m_notifier->textNotify(text);
  emit receivedInput(text);
}

void Console::displayText(QString text, int offset) {
  if (offset == 0 && promptRegexp.indexIn(text) == 0) {
    prompt->setText(" " + promptRegexp.cap(1));
    offset += promptRegexp.cap(1).length();
  }

  text = text.mid(offset);
  text.replace("\t", "    ");

  QByteArray bytes = text.toAscii();
  if (bytes.contains("\xff\xfc\x01")) {
    edit->setEchoMode(QLineEdit::Normal);
  }
  else if (bytes.contains("\xff\xfb\x01")) {
    edit->setEchoMode(QLineEdit::Password);
  }

  if (text[offset].isPrint()) {
    ConsoleOutput output(display);
    m_highlighter(text, output);
  }
}

void Console::clear() {
  display->clear();
}


#include "console.moc"


