/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENGINE_H
#define ENGINE_H

#include <KProcess>

#include <boost/weak_ptr.hpp>
#include <queue>

#include "console.h"
#include "tagua.h"


class EngineNotifier;

/**
  * @brief An external chess engine.
  *
  * This class incapsulates a connection with
  * an external engine process in a protocol
  * independent way.
  */
class Engine : public QObject, public TextNotifier {
Q_OBJECT
protected:
  QString m_path;
  QStringList m_arguments;
  KProcess m_engine;
  QString m_workPath;
  boost::shared_ptr<Console> m_console;
  boost::weak_ptr<EngineNotifier> m_notifier;
  std::queue<QString> m_command_queue;

  virtual void initializeEngine() = 0;
public:
  /**
    * Create an engine, setting the path to the engine
    * program and its arguments.
    */
  Engine(const QString& path, const QStringList& arguments);

  void setWorkingPath(const QString& workPath) { m_workPath = workPath; }

  virtual ~Engine();

  /**
    * Start the engine program. The initialization will
    * take place automatically after the program has started.
    */
  void start();

  /**
    * Send a command to the engine.
    */
  void sendCommand(const QString& command, bool echo = true);

  /**
    * Send a move to the engine.
    */
  virtual void sendMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref) = 0;

  /**
    * Back up a move.
    * @param pos The position before the move. Used if the engine
    *            does not support the undo command.
    */
  virtual void backUp(AbstractPosition::Ptr pos) = 0;

  virtual void setBoard(AbstractPosition::Ptr pos, int halfmove, int fullmove) = 0;

  virtual void startAnalysis() = 0;
  virtual void stopAnalysis() = 0;

  /**
    * Begin a new game.
    */
  virtual void reset() = 0;
  
  /**
    * Start playing.
    */
  virtual void play() = 0;

  /**
    * Stop the engine.
    */
  virtual void stop() = 0;

  /**
    * Receive a user command.
    */
  virtual void textNotify(const QString& text);

  void setNotifier(const boost::shared_ptr<EngineNotifier>& notifier);
  void setConsole(const boost::shared_ptr<Console>& console);
private Q_SLOTS:
  void engineRunning();
  void processInput();

Q_SIGNALS:

  /**
    * Emitted when the engine sends a command to the interface.
    */
  void receivedCommand(const QString& command);

  /**
    * Emitted when the engine exits (or crashes, etc...).
    */
  void lostConnection();
};

#endif // ENGINE_H
