/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GAME_H
#define GAME_H

#include <boost/shared_ptr.hpp>
#include <boost/variant/variant_fwd.hpp>
#include <vector>
#include "kboard_fwd.h"
#include "index.h"

class PGN;

namespace GamePrivate {

class Entry;
typedef std::vector<Entry> History;

typedef boost::variant<class UndoAdd, class UndoPromote, class UndoTruncate,
                 class UndoRemove, class UndoClear, class UndoSetComment> UndoOp;
typedef std::vector<UndoOp> UndoHistory;

}


class Game {
public:
  GamePrivate::History history;
  Index current;

  GamePrivate::UndoHistory undo_history;
  int undo_pos;

  GamePrivate::Entry* fetch(const Index& ix);
  const GamePrivate::Entry* fetch(const Index& ix) const;
  GamePrivate::History* fetchRef(const Index& ix, int* idx);
  const GamePrivate::History* fetchRef(const Index& ix, int* idx) const;

  void testMove();
  void testMove(const Index& ix);
  void saveUndo(const GamePrivate::UndoOp& op);

  QString variationPgn(const GamePrivate::History&, const GamePrivate::Entry&,
                          int start, const Index& _ix) const;

  virtual void onAdded(const Index& i);
  virtual void onRemoved(const Index& i);
  virtual void onEntryChanged(const Index& at, int propagate=1);
  virtual void onPromoteVariation(const Index& i, int v);
  virtual void onSetComment(const Index& i, const QString& c);
  virtual void onSetVComment(const Index& i, int v, const QString& c);
  virtual void onCurrentIndexChanged(const Index& old = Index(-1));
  virtual void onAvailableUndo(bool);
  virtual void onAvailableRedo(bool);

public:
  Game();
  virtual ~Game();

  Index index() const;
  Index lastMainlineIndex() const;
  bool containsIndex(const Index& index) const;
  MovePtr move() const;
  MovePtr move(const Index& index) const;
  PositionPtr position() const;
  PositionPtr position(const Index& index) const;
  QString comment() const;
  QString comment(const Index& index) const;
  void reset(PositionPtr);
  void undo();
  void redo();
  void setComment(const QString& c);
  void setComment(const Index& index, const QString& c);
  void setVComment(const Index& index, int v, const QString& c);
  void promoteVariation();
  void promoteVariation(const Index& index);
  void promoteVariation(const Index& index, int v);
  void removeVariation(int v);
  void removeVariation(const Index& index);
  void removeVariation(const Index& index, int v);
  void clearVariations();
  void clearVariations(const Index& index);
  void truncate();
  void truncate(const Index& index);
  void add(MovePtr move, PositionPtr pos);
  bool insert(MovePtr move, PositionPtr pos, const Index& index);
  bool lastPosition() const;
  bool back();
  bool forward();
  void gotoFirst();
  void gotoLast();
  bool goTo(const Index& index);
  QString pgn() const;
  void load(PositionPtr, const PGN& pgn);
  void load(const PGN& pgn);
};

#endif // GAME_H
