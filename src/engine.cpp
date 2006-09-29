/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "engine.h"

#include <QTextStream>

#define ENGINE_DEBUG
#ifdef ENGINE_DEBUG
  #include <iostream>
  #include "common.h"
#endif

using namespace boost;

Engine::Engine(const QString& path, const QStringList& arguments)
: m_path(path)
, m_arguments(arguments) {
  connect(&m_engine, SIGNAL(readyRead()), this, SLOT(processInput()));
  connect(&m_engine, SIGNAL(finished(int, QProcess::ExitStatus)), 
          this, SIGNAL(lostConnection()));
}

Engine::~Engine() { }

void Engine::start() {
  if (!m_workPath.isNull())
    m_engine.setWorkingDirectory(m_workPath);
  m_engine.start(m_path, m_arguments);
  initializeEngine();
}

void Engine::sendCommand(const QString& command, bool echo) {
  QTextStream os(&m_engine);
  os << command << "\n";
  if (echo && m_console)
    m_console->echo(command);
#ifdef ENGINE_DEBUG
  std::cout << "> " << command << std::endl;
#endif
}

void Engine::processInput() {
  // process only full lines
  while (m_engine.canReadLine()) {
    QString line = m_engine.readLine();
    line.remove("\n").remove("\r");
#ifdef ENGINE_DEBUG
    std::cout << "< " << line << std::endl;
#endif
    if (m_console)
      m_console->displayText(line + "\n", 0);
    emit receivedCommand(line);
  }
}

void Engine::textNotify(const QString& text) {
  sendCommand(text, false);
}


void Engine::setNotifier(const shared_ptr<EngineNotifier>& notifier) {
  m_notifier = notifier;
}

void Engine::setConsole(const shared_ptr<Console>& console) {
  m_console = console;
}
#include "engine.moc"
