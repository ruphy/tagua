/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GAME_P_H
#define GAME_P_H

#include <map>
#include <QString>
#include "game.h"

namespace GamePrivate {

typedef std::map<int, History> Variations;
typedef std::map<int, QString> VComments;

class Entry {
public:
  MovePtr move;
  PositionPtr position;
  QString comment;
  Variations variations;
  VComments vcomments;
  int last_var_id;

  Entry(MovePtr move, PositionPtr position)
    : move(move)
    , position(position)
    , last_var_id(0) { }
  Entry()
    : last_var_id(0) { }
  ~Entry() { }
};

/* data and structs for undo */
class UndoAdd {
public:
  Index index;
  Entry entry;
  UndoAdd(const Index& ix, const Entry& e)
    : index(ix), entry(e) {}
};

class UndoPromote {
public:
  Index index;
  int variation;
  UndoPromote(const Index& ix, int v)
    : index(ix), variation(v) {}
};

class UndoTruncate {
public:
  Index index;
  History history;
  Variations variations;
  VComments vcomments;
  UndoTruncate(const Index& ix)
    : index(ix) {}
  UndoTruncate(const Index& ix, const History& h,
          const Variations& v, const VComments& c)
    : index(ix), history(h), variations(v), vcomments(c) {}
};

class UndoRemove {
public:
  Index index;
  int variation;
  History history;
  QString vcomment;
  UndoRemove(const Index& ix, int v, const History& h, const QString& c)
    : index(ix), variation(v), history(h), vcomment(c) {}
};
class UndoClear {
public:
  Index index;
  Variations variations;
  VComments vcomments;
  UndoClear(const Index& ix, const Variations& v, const VComments& c)
    : index(ix), variations(v), vcomments(c) {}
};

class UndoSetComment {
public:
  Index index;
  int variation;
  QString old_comment;
  QString new_comment;
  UndoSetComment(const Index& ix, int v, const QString& o, const QString& n)
    : index(ix), variation(v), old_comment(o), new_comment(n) {}
};

}

#endif //GAME_P_H


