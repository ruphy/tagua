/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "xboardengine.h"

#include <QFileInfo>
#include <QRegExp>
#include <iostream>

#include "enginenotifier.h"
#include "foreach.h"

using namespace boost;

QRegExp XBoardEngine::m_move_pattern("My move is: (.*)");

XBoardEngine::XBoardEngine(const QString& path, const QStringList& arguments)
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
  m_features.analyze = true;
  m_features.colors = true;
  m_features.ics = false;
  m_features.name = false;
  m_features.pause = false;

  m_features.myname = QFileInfo(m_path).baseName();

  connect(this, SIGNAL(receivedCommand(const QString&)),
          this, SLOT(processCommand(const QString&)));
}

XBoardEngine::~XBoardEngine() {
  std::cout << "[debug] destroying engine" << std::endl;
  stop();
  m_engine.terminate();
}

void XBoardEngine::initializeEngine() {
  // set up in xboard mode
  sendCommand("xboard");

  // we're using xboard protocol version 2
  sendCommand("protover 2");

  // don't flood me!
  sendCommand("nopost");
}

void XBoardEngine::reset(int side) {
  sendCommand("force");
  sendCommand("new");
  if (side == 0 && !m_analysing) sendCommand("go");
}

void XBoardEngine::stop() {
  sendCommand("quit");
  if (m_features.sigterm)
    m_engine.kill();
}

void XBoardEngine::processCommand(const QString& command) {
  QStringList arg_list = command.split(QRegExp("\\s+"));
  QString cmd = arg_list.takeFirst();

  if (cmd == "feature") {
    QRegExp feature("(\\S+)=(\\S+)");
    foreach (QString arg, arg_list) {
      if (feature.indexIn(arg) == 0) {
        bool rejected = false;

        QString key = feature.cap(1);
        QString value = feature.cap(2);

        if (key == "ping")
          m_features.ping = value == "1";
        else if (key == "setboard")
          m_features.setboard = value == "1";
        else if (key == "playother")
          m_features.playother = value == "1";
        else if (key == "san")
          m_features.san = value == "1";
        else if (key == "usermove")
          m_features.usermove = value == "1";
        else if (key == "time")
          m_features.time = value == "1";
        else if (key == "draw")
          m_features.draw = value == "1";
        else if (key == "sigint")
          m_features.sigint = value == "1";
        else if (key == "sigterm")
          m_features.sigterm = value == "1";
        else if (key == "reuse")
          m_features.reuse = value == "1";
        else if (key == "analyze")
          m_features.analyze = value == "1";
        else if (key == "myname")
          m_features.myname = value.mid(1, value.length() - 2);
        else if (key == "variants")
          m_features.variants = value.mid(1, value.length() - 2);
        else if (key == "colors")
          m_features.colors = value == "1";
        else if (key == "ics")
          m_features.ics = value == "1";
        else if (key == "name")
          m_features.name = value == "1";
        else if (key == "pause")
          m_features.pause = value == "1";
        else if (key == "done")
          m_features.done = value == "1";
        else
          rejected = true;

        if (rejected)
          sendCommand("rejected " +  key);
        else
          sendCommand("accepted " + key);
      }
    }
  }
  else if (cmd == "move") {
    if (shared_ptr<EngineNotifier> notifier = m_notifier.lock())
      notifier->notifyEngineMove(arg_list[0]);
  }
  else if (m_move_pattern.indexIn(command) == 0) {
    if (shared_ptr<EngineNotifier> notifier = m_notifier.lock())
      notifier->notifyEngineMove(m_move_pattern.cap(1));
  }
}

void XBoardEngine::sendMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref) {
  QString move_str = m_features.san ? move->SAN(ref) : move->toString(ref);
  if (m_features.usermove)
    move_str = "usermove " + move_str;
  sendCommand(move_str);
}

void XBoardEngine::backUp(AbstractPosition::Ptr) {
  sendCommand("undo");
}

void XBoardEngine::startAnalysis() {
  sendCommand("post");
  sendCommand("analyze");
  m_analysing = true;
}

void XBoardEngine::stopAnalysis() {
  sendCommand("exit");
  m_analysing = false;
}

void XBoardEngine::setBoard(AbstractPosition::Ptr pos, int halfmove, int fullmove) {
  if (m_features.setboard) {
    sendCommand(QString("setboard %1").arg(pos->fen(halfmove, fullmove)));
  }
  else {
  // this is pretty meaningless for generic variants
  #if 0
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
  #endif
  }
}

