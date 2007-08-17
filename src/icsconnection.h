/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSCONNECTION_H
#define ICSCONNECTION_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QStringList>
#include "connection.h"
#include "icsgamedata.h"

class GameInfo;
class PGN;
class PositionInfo;
class PoolInfo;
class ICSListener;

class ICSConnection : public Connection {
Q_OBJECT
  typedef std::map<int, ICSGameData> GameList;
  
  enum State {
    Normal,
    MoveListHeader,
    MoveListMoves
  } state;

  // patterns
  static QRegExp pressReturn;
  static QRegExp creating;
  static QRegExp game;
  static QRegExp login;
  static QRegExp password;
  static QRegExp fics;
  static QRegExp beep;
  static QRegExp unexamine, unobserve;
  static QRegExp move_list_start, move_list_players, move_list_game,
                 move_list_ignore1, move_list_ignore2, move_list_terminator;
  static QRegExp observed_game;
  static QRegExp goForward, goBack;

  GameInfo* incomingGameInfo;
  int m_processed_offset;
  GameList m_games;

  int m_move_list_game_num;
  QStringList m_move_list_players;
  GameInfo *m_move_list_game_info;
  PositionInfo *m_move_list_position_info;
  PoolInfo *m_move_list_pool_info;
  QString m_move_list;

  bool test(const QRegExp& pattern, const QString& str);
public:
  ICSConnection();

  void setListener(int gameNumber, const boost::weak_ptr<ICSListener>& listener);
  void startup();
public Q_SLOTS:
  void process(QString str);
  void processPartialLine(QString str);
Q_SIGNALS:
  void notification();
  void moves(const PGN&);
  void time(int, int);
  void loginPrompt();
  void passwordPrompt();
  void prompt();
  void registeredNickname();
  void creatingGame(const GameInfo*, const PositionInfo&);
  void creatingExaminedGame(const GameInfo*, const PositionInfo&);
  void creatingObservedGame(const GameInfo*, const PositionInfo&);
  void endingGame(const QString&, const QString&);
  void endingExaminedGame(int);
  void endingObservedGame(int);
};

#endif // ICSCONNECTION_H
