/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef XBOARDENGINE_H
#define XBOARDENGINE_H

#include "engine.h"
#include "tagua.h"

class QRegExp;

/**
  * @brief A chess engine compatible with xboard.
  *
  * XBoard compatible engines are defined by Tim Mann's
  * Chess Engine Communication Protocol (available at
  * http://tim-mann.org/xboard/engine-intf.html), used
  * by GNU Chess, Crafty and many other free and commercial
  * chess engines.
  * This class encapsulates such communication protocol,
  * calling appropriate methods of the notifier upon engine
  * generated events.
  *
  * @note This code is partially stolen from Maurizio Monge's
  *       old interface project outoftime.
  */
class XBoardEngine : public Engine {
Q_OBJECT
  struct Features {
    bool ping         : 1;
    bool setboard     : 1;
    bool playother    : 1;
    bool san          : 1;
    bool usermove     : 1;
    bool time         : 1;
    bool draw         : 1;
    bool sigint       : 1;
    bool sigterm      : 1;
    bool reuse        : 1;
    bool analyze      : 1;
    QString myname;
    QString variants;
    bool colors       : 1;
    bool ics          : 1;
    bool name         : 1;
    bool pause        : 1;
    bool done         : 1;
  };

  Features m_features;
  bool m_analysing;

  static QRegExp m_move_pattern;
protected:
  /**
    * Initialize the engine.
    */
  virtual void initializeEngine();
protected Q_SLOTS:
  /**
    * Parse engine command line and the appropriate
    * signals.
    */
  void processCommand(const QString& command);
public:
  /**
    * Create an xboard compatible engine.
    */
  XBoardEngine(const QString& path, const QStringList& arguments);

  /**
    * Terminate the engine.
    */
  virtual ~XBoardEngine();

  /**
    * Send a move to the engine.
    */
  virtual void sendMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref);

  /**
    * Back up a move.
    * @param pos The position before the move. Used if the engine
    *            does not support the undo command.
    */
  virtual void backUp(AbstractPosition::Ptr pos);

  virtual void setBoard(AbstractPosition::Ptr pos, int halfmove, int fullmove);


  /**
    * Begin a new game.
    */
  virtual void reset();
  
  /**
    * Start playing.
    */
  virtual void play();

  /**
    * Stop the engine.
    */
  virtual void stop();

  virtual void startAnalysis();
  virtual void stopAnalysis();
};

#endif // XBOARDENGINE_H
