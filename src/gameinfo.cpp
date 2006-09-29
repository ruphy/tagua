/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "gameinfo.h"


GameInfo::GameInfo(const Player& white, const Player& black,
                   const QString& rated, const QString& type,
                   int time, int increment)
: m_white(white)
, m_black(black)
, m_rated(rated)
, m_type(type)
, m_time(time)
, m_increment(increment)
, m_game_num(-1) {
  m_variant = variantCode(m_type);
}

GameInfo::GameInfo(QRegExp& pattern, int offset)
: m_white(Player(pattern.cap(offset), pattern.cap(offset + 1).toInt()))
, m_black(Player(pattern.cap(offset + 2), pattern.cap(offset + 3).toInt()))
, m_rated(pattern.cap(offset + 4))
, m_type(pattern.cap(offset + 5))
, m_time(pattern.cap(offset + 6).toInt())
, m_increment(pattern.cap(offset + 7).toInt())
, m_game_num(-1) {
  m_variant = variantCode(m_type);
}

QString GameInfo::variantCode(const QString& type) {
  if (type == "blitz" || type == "standard" || type == "lightning"
      || type == "untimed" || type == "chess")
    return "Chess";
  else if (type == "atomic")
    return "Atomic";
  else if (type == "crazyhouse")
    return "Crazyhouse";
  else
    return "Dummy";
}
