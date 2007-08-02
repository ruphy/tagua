/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GNUSHOGIENGINE_H
#define GNUSHOGIENGINE_H

#include "engine.h"
#include "tagua.h"

class QRegExp;

/**
  * @brief A shogi engine compatible with gnushogi.
  *
  * @note This code is partially stolen from the XBoard engine class
  */
class GNUShogiEngine : public Engine {
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
    * Create an gnushogi compatible engine.
    */
  GNUShogiEngine(const QString& path, const QStringList& arguments);

  /**
    * Terminate the engine.
    */
  virtual ~GNUShogiEngine();

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

#endif // GNUSHOGIENGINE_H
