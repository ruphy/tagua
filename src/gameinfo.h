/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GAMEINFO_H
#define GAMEINFO_H

#include <qregexp.h>
#include "player.h"
#include "qstring.h"

class GameInfo {
  Player m_white, m_black;
  QString m_rated, m_type;
  QString m_variant;
  int m_time, m_increment;
  int m_game_num;

public:
  GameInfo(const Player& white, const Player& black,
            const QString& rated, const QString& type,
            int time, int increment);

  GameInfo(QRegExp& pattern, int offset);

  static QString variantCode(const QString& type);
  const Player& white() const { return m_white; }
  const Player& black() const { return m_black; }
  QString rated() const { return m_rated; }
  QString type() const { return m_type; }
  int time() const { return m_time; }
  int increment() const { return m_increment; }
  int gameNumber() const { return m_game_num; }
  void setGameNumber(int g) { m_game_num = g; }
  QString variant() const { return m_variant; }
};

#endif // GAMEINFO_H

