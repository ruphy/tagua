/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef USERENTITY_H
#define USERENTITY_H

#ifndef GAMEBASEDRULES_H
#define GAMEBASEDRULES_H

#include "entity.h"
#include "kboard.h"
#include "usermove.h"
#include "agent.h"

class TurnTest {
public:
  virtual ~TurnTest() { }
  virtual bool operator()(int turn) const = 0;
};

class FreeTurnTest : public TurnTest {
public:
  virtual bool operator()(int) const { return true; }
};

class NoTurnTest : public TurnTest {
public:
  virtual bool operator()(int) const { return false; }
};

class OneTurnTest : public TurnTest {
  int m_turn;
public:
  OneTurnTest(int turn)
  : m_turn(turn) { }
  virtual bool operator()(int turn) const {
    return turn == m_turn;
  }
};

class UserEntity : public Entity
                 , public Agent {
protected:
  boost::shared_ptr<TurnTest> m_turn_test;
  bool m_editing_tools;
  int m_promotion;
public:
  UserEntity(int);
  virtual void setTurnTest(const boost::shared_ptr<TurnTest>& test) { m_turn_test = test; }
  virtual void enableEditingTools(bool value);

  // promotion
  void changePromotionType(int type) { m_promotion = type; }
  void setPromotion(NormalUserMove& move) { move.promotionType = m_promotion; }
  int promotionType() const { return m_promotion; }

  /**
    * Return a PGN for the game.
    */
  virtual QString save() const = 0;

  /**
    * Load the content of a PGN inside the game.
    */
  virtual void loadPGN(const QString& pgn) = 0;

  virtual AbstractMove::Ptr testMove(const NormalUserMove& m) const = 0;
  virtual AbstractMove::Ptr testMove(const DropUserMove& m) const = 0;
  virtual bool testPremove(const NormalUserMove& m) const = 0;
  virtual bool testPremove(const DropUserMove&) const = 0;
  virtual void executeMove(AbstractMove::Ptr move) = 0;
  virtual void addPremove(const NormalUserMove& m) = 0;
  virtual void addPremove(const DropUserMove& m) = 0;
  virtual void cancelPremove() = 0;
  virtual void handleRightClick(const Point&) const;

  virtual Action validTurn(const Point&) const = 0;
  virtual Action validTurn(int turn) const  = 0;
  virtual bool movable(const Point&) const = 0;
  virtual bool oneClickMoves() const { return false; }

  /**
    * Jump to the end of the game.
    */
  virtual bool gotoFirst() = 0;

  /**
    * Jump to the beginning of the game.
    */
  virtual bool gotoLast() = 0;

  /**
    * Jump to an arbitrary index.
    */
  virtual bool goTo(const Index& index) = 0;

  /**
    * Go forward one move in the game.
    */
  virtual bool forward() = 0;

  /**
    * Go back one move in the game.
    */
  virtual bool back() = 0;

  /**
    * Undo the last editing action.
    */
  virtual bool undo() = 0;

  /**
    * Redo the last editing action.
    */
  virtual bool redo() = 0;

  /**
    * Remove the current position.
    */
  virtual bool truncate() = 0;

  /**
    * Promote the current variation to mainline.
    */
  virtual bool promoteVariation() = 0;

  /**
    * Move hint.
    * @sa AbstractPosition::moveHint
    */
  virtual AbstractPiece::Ptr moveHint(AbstractMove::Ptr) const { return AbstractPiece::Ptr(); }

  virtual bool canDetach() const;
};


#endif // GAMEBASEDRULES_H


#endif // USERENTITY_H