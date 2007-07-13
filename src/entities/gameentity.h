/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GAMEENTITY_H
#define GAMEENTITY_H

#include "userentity.h"
#include "agent.h"
#include "agentgroup.h"

class Board;

class GameEntity : public UserEntity {
  VariantInfo* m_variant;
  Board* m_chessboard;
  AgentGroupDispatcher m_dispatcher;
  boost::shared_ptr<class Premove> m_premoveQueue;

  AbstractPosition::Ptr doMove(AbstractMove::Ptr move) const;
public:
  GameEntity(VariantInfo* variant, const boost::shared_ptr<Game>& game,
                 Board* chessboard, AgentGroup* group);

  /**
    * Return a PGN for the game.
    */
  virtual QString save() const;

  /**
    * Load the content of a PGN inside the game.
    */
  virtual void loadPGN(const PGN& pgn);

  virtual void executeMove(AbstractMove::Ptr move);
  virtual void addPremove(const NormalUserMove& m);
  virtual void addPremove(const DropUserMove& m);
  virtual void cancelPremove();

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref);
  virtual void notifyBack();
  virtual void notifyForward();
  virtual void notifyGotoFirst();
  virtual void notifyGotoLast();

  virtual AbstractMove::Ptr testMove(const NormalUserMove&) const;
  virtual AbstractMove::Ptr testMove(const DropUserMove&) const;
  virtual AbstractPiece::Ptr moveHint(AbstractMove::Ptr move) const;
  virtual bool testPremove(const NormalUserMove&) const;
  virtual bool testPremove(const DropUserMove&) const;
  virtual InteractionType validTurn(const Point&) const;
  virtual InteractionType validTurn(int) const;
  virtual bool movable(const Point&) const;
  virtual bool oneClickMoves() const;

  virtual bool gotoFirst();
  virtual bool gotoLast();
  virtual bool goTo(const Index& index);
  virtual bool forward();
  virtual bool back();
  virtual bool undo();
  virtual bool redo();
  virtual bool truncate();
  virtual bool promoteVariation();

  virtual bool canDetach() const;
};

#endif // GAMEENTITY_H
