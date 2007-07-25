/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef POOLINFO_H
#define POOLINFO_H

#include <boost/shared_ptr.hpp>
#include <QRegExp>
#include "index.h"
#include "tagua.h"
#include "icsgamedata.h"

class PoolInfo {
public:
  bool  m_valid;
  int   m_game_num;
  Index m_pos_index;
  PoolPtr m_pool[2];
  PiecePtr m_added_piece;
  static QRegExp s_pattern;

  PoolInfo(const std::map<int, ICSGameData>& games, const QString& s);
};

#endif //POOLINFO_H
