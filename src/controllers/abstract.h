/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <boost/enable_shared_from_this.hpp>
#include <QString>
#include <KUrl>
#include "common.h"
#include "entitytoken.h"
#include "fwd.h"

class ChessTable;
class UserEntity;
class Engine;
class PGN;
class ActionCollection;
class UI;

/**
  * @b Controller is the base abstract class for all controllers in Tagua.
  * Tagua can be used in different modes, such as game editing or position editing.
  * Each mode corresponds to a concrete subclass of Controller, and defines its behaviour
  * in response to user events (history navigation, move executions...).
  * By default, events are routed to the user entity returned by the virtual function
  * entity.
  */
class Controller : public boost::enable_shared_from_this<Controller> {
protected:
  ChessTable* m_view;
  virtual boost::shared_ptr<UserEntity> entity() const = 0;
  KUrl m_url;
public:
  Controller(ChessTable* view);
  virtual ~Controller();

  /**
    * Variant specific actions.
    */
  virtual ActionCollection* variantActions() const = 0;
  
  /**
    * Variant associated to the controller.
    */
  virtual QString variant() const = 0;

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
    * @return a token which uniquely identifies the engine entity
    *         and can be used to remove it from the entity list.
    */
  virtual EntityToken addPlayingEngine(int /*side*/, const boost::shared_ptr<Engine>&) { return EntityToken(); }

  /**
    * Remove an entity from the controller list.
    */
  virtual void removeEntity(const EntityToken&) { }
  
  virtual void reloadSettings() = 0;
  
  /**
    * Set owning user interface.
    * Used to setup callbacks towards the main window, like action state
    * change notifications.
    */
  virtual void setUI(UI& ui) = 0;
  
  /**
    * Called whenever a controller becomes active in the main window.
    */
  virtual void activate() = 0;
  
  /**
    * Each controller has an associated URL that will be used for saving
    * its content.
    * @return The URL associated with this controller.
    */
  virtual KUrl url() const;
  
  /**
    * Set this controller's URL.
    * @sa url()
    */
  virtual void setUrl(const KUrl& url);
};


#endif
