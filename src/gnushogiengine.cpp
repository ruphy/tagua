/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "gnushogiengine.h"

#include <QFileInfo>
#include <QRegExp>
#include <KDebug>
#include "enginenotifier.h"
#include "foreach.h"

using namespace boost;

QRegExp GNUShogiEngine::m_move_pattern("[0-9]*\\. \\.\\.\\. ([^ ]*) .*");

GNUShogiEngine::GNUShogiEngine(const QString& path, const QStringList& arguments)
: Engine(path, arguments)
, m_analysing(false) {
  // set default features
  m_features.ping = false;
  m_features.setboard = false;
  m_features.playother = false;
  m_features.san = false;
  m_features.usermove = false;
  m_features.time = true;
  m_features.draw = true;
  m_features.sigint = true;
  m_features.sigterm = true;
  m_features.reuse = true;
  m_features.analyze = false;
  m_features.colors = true;
  m_features.ics = false;
  m_features.name = false;
  m_features.pause = false;

  m_features.myname = QFileInfo(m_path).baseName();

  connect(this, SIGNAL(receivedCommand(const QString&)),
          this, SLOT(processCommand(const QString&)));
}

GNUShogiEngine::~GNUShogiEngine() {
  kDebug() << "[debug] destroying engine";
  stop();
  m_engine.kill();
}

void GNUShogiEngine::initializeEngine() {
  // nothing special to do
}

void GNUShogiEngine::reset() {
  sendCommand("new"); // start a new game
  sendCommand("force"); // do not think
}

void GNUShogiEngine::play() {
  sendCommand("go"); // start playing
}

void GNUShogiEngine::stop() {
  sendCommand("quit");
  if (m_features.sigterm)
    m_engine.kill();
}

void GNUShogiEngine::processCommand(const QString& command) {
  //QStringList arg_list = command.split(QRegExp("\\s+"));
  //QString cmd = arg_list.takeFirst();

  if (m_move_pattern.indexIn(command) == 0) {
    if (shared_ptr<EngineNotifier> notifier = m_notifier.lock())
      notifier->notifyEngineMove(m_move_pattern.cap(1));
  }
}

void GNUShogiEngine::sendMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref) {
  QString move_str = move->toString("simple", ref);
  sendCommand(move_str);
}

void GNUShogiEngine::backUp(AbstractPosition::Ptr) {
  sendCommand("undo");
}

void GNUShogiEngine::startAnalysis() {
  sendCommand("post");
  sendCommand("analyze");
  m_analysing = true;
}

void GNUShogiEngine::stopAnalysis() {
  sendCommand("exit");
  m_analysing = false;
}

void GNUShogiEngine::setBoard(AbstractPosition::Ptr, int, int) {
  #if 0
  if (m_features.setboard) {
    sendCommand(QString("setboard %1").arg(pos->fen(halfmove, fullmove)));
  }
  else {
  // this is pretty meaningless for generic variants
    if (pos->turn() != 0) {
      sendCommand("new");
      sendCommand("a2a3");
    }

    sendCommand("edit");
    sendCommand("#");
    Point size = pos->size();
    for (int i = 0; i < size.x; i++) {
      for (int j = 0; j < size.y; j++) {
        Point p(i, j);
        AbstractPiece::Ptr piece = pos->get(p);
        if (piece) sendCommand(QString("%1%2")
                      .arg(piece->type())
                      .arg(p.toString(8)));
      }
    }
    sendCommand(".");
  }
  #endif
}

