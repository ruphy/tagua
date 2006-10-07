/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <boost/enable_shared_from_this.hpp>
#include <qstring.h>
#include "common.h"
#include "entitytoken.h"
#include "kboard_fwd.h"

class GenericPosition;
class PositionInfo;
class Connection;
class ChessTable;
class GenericGame;
class UserEntity;
class Engine;
class PGN;

/**
  * @b Controller is the base abstract class for all controllers in KBoard.
  * KBoard can be used in different modes, such as game editing or position editing.
  * Each mode corresponds to a concrete subclass of Controller, and defines its behaviour
  * in response to user events (history navigation, move executions...).
  * By default, events are routed to the user entity returned by the virtual function
  * entity.
  */
class Controller : public boost::enable_shared_from_this<Controller> {
protected:
  ChessTable* m_view;
  virtual boost::shared_ptr<UserEntity> entity() const = 0;
public:
  Controller(ChessTable* view)
  : m_view(view) { }
  virtual ~Controller() { }

  /**
    * Terminate activity, create another controller and delegate everything to it.
    * @return The delegate controller, or simply @b this if controller does not need
    * to be terminated.
    */
  virtual boost::shared_ptr<Controller> end() { return shared_from_this(); }

  /**
    * Detach from resources without disposing them.
    */
  virtual void detach() { }

  /**
    * Clear board. Undefined for almost all controllers.
    * @return Whether the action has been successfully executed.
    */
  virtual bool clearBoard() { return false; }

  /**
    * Set starting position on the board. Undefined for almost all controllers.
    * @return Whether the action has been successfully executed.
    */
  virtual bool setStartingPosition() { return false; }

  /**
    * Set a position on the board. Undefined for almost all controllers.
    * @param fen The position to be set, in FEN notation.
    * @return Whether the action has been successfully executed.
    */
  virtual bool setFEN(const QString&) { return false; }

  /**
    * Get a FEN string representing the current position on the board.
    */
  virtual QString fen() { return ""; }

  /**
    * @return current position, or a null shared_ptr if no current position
    * concept is defined for this controller.
    */
  virtual PositionPtr currentPosition() const { return PositionPtr(); }

  /**
    * Set autopromotion type. In chess and variants with promotions,
    * all pawns will be automatically promoted to @a type.
   */
  virtual void setPromotionType(int);

  /**
    * Retrieve current promotion type.
    * @sa setPromotionType.
    */
  virtual int promotionType() const;

  /**
    * Change turn. Used for example in edit position mode.
    */
  virtual void setTurn(int) { }

  /**
    * Forward a movelist undo call.
    */
  virtual bool undo();

  /**
    * Forward a movelist redo call.
    */
  virtual bool redo();

  /**
    * Forward a movelist truncate call.
    */
  virtual bool truncate();

  /**
    * Forward a movelist promote variation call.
    */
  virtual bool promoteVariation();

  /**
   * Navigate back in history.
   */
  virtual bool back();

  /**
   * Navigate forward in history.
   */
  virtual bool forward();

  /**
   * Warp to the beginning of the history.
   */
  virtual void gotoFirst();

  /**
   * Warp to the end of the history.
   */
  virtual void gotoLast();

  /**
    * Save game.
    */
  virtual QString save();

  /**
    * Load game from a PGN.
    */
  virtual void loadPGN(const PGN&);

  /**
    * Create a CTRL Action.
    * @sa UI::createCtrlAction
    */
  virtual void createCtrlAction() { }

  /**
    * Destroy a CTRL Action.
    * @sa UI::createCtrlAction.
    */
  virtual void destroyCtrlAction() { };

  /**
    * Let an engine play as player @a side.
    */
  virtual bool addPlayingEngine(int /*side*/, const boost::shared_ptr<Engine>&) { return false; }

  /**
    * Start the engine in analysis mode.
    * @return a token which uniquely identifies the analysing engine entity
    *         and can be used to remove it from the entity list.
    */
  virtual EntityToken addAnalysingEngine(const boost::shared_ptr<Engine>&) { return EntityToken(); }

  /**
    * Stop an analysing engine entity.
    */
  virtual void removeAnalysingEngine(const EntityToken&) { }
};


#endif
