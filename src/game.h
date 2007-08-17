/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
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
#include "fwd.h"
#include "index.h"

class PGN;

namespace GamePrivate {

class Entry;
typedef std::vector<Entry> History;

typedef boost::variant<class UndoAdd, class UndoPromote, class UndoTruncate,
                 class UndoRemove, class UndoClear, class UndoSetComment> UndoOp;
typedef std::vector<UndoOp> UndoHistory;

}


/**
    \class Game game.h <game.h>
    \brief A game with history and variations.

    This template class encapsulates an editable game with history and undo editing.
*/
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
  /** Constructor, creates an empty game*/
  Game();

  /** destructor */
  virtual ~Game();

  /** \return the index of the current position */
  Index index() const;

  /** \return an index opinting to the last position in the main line */
  Index lastMainlineIndex() const;

  /** \return true if the game contains the index */
  bool containsIndex(const Index& index) const;

  /** \return the current move */
  MovePtr move() const;

  /** \return the move at the given index */
  MovePtr move(const Index& index) const;

  /** \return the current position */
  PositionPtr position() const;

  /** \return the position at the given index */
  PositionPtr position(const Index& index) const;

  /** \return the current comment */
  QString comment() const;

  /** \return the comment at the given index */
  QString comment(const Index& index) const;

  /** clears the games, and puts \a pos as root position */
  void reset(PositionPtr pos);

  /** undo */
  void undo();

  /** redo */
  void redo();

  /** sets the comment in the current index */
  void setComment(const QString& c);

  /** sets the comment at the given index */
  void setComment(const Index& index, const QString& c);

  /** sets the variation comment at the given index/variation */
  void setVComment(const Index& index, int v, const QString& c);

  /** promotes the current position in the upper main line */
  void promoteVariation();

  /** promotes the given position in the upper main line */
  void promoteVariation(const Index& index);

  /** promotes the given variation in the upper main line */
  void promoteVariation(const Index& index, int v);

  /** removes the given variation in the current index */
  void removeVariation(int v);

  /** removes the given variation in the given index */
  void removeVariation(const Index& index);

  /** removes the given variation in the given index */
  void removeVariation(const Index& index, int v);

  /** removes the given variation in the current index */
  void clearVariations();

  /** removes the given variation in the given index */
  void clearVariations(const Index& index);

  /** removes all the successors of the current position */
  void truncate();

  /** removes all the successors of the given position */
  void truncate(const Index& index);

  /** adds a new move+position after the current one, on the main
    line if possible, or else in a new variation */
  void add(MovePtr move, PositionPtr pos);

  /** forces a move+position at in certain index */
  bool insert(MovePtr move, PositionPtr pos, const Index& index);

  /** \return true if we cannot go forward */
  bool lastPosition() const;

  /** go back */
  bool back();

  /** go forward (in the current mainline) */
  bool forward();

  /** go to the root position */
  void gotoFirst();

  /** go to the last position (in the current mainline) */
  void gotoLast();

  /** go to a specified index */
  bool goTo(const Index& index);

  /** \return a pgn containing the whole game (with variations) */
  QString pgn() const;

  /** loads a pgn in the current game */
  void load(PositionPtr, const PGN& pgn);

  /** loads a pgn in the current game */
  void load(const PGN& pgn);
};

#endif // GAME_H
