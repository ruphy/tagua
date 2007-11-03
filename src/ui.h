/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef UI_H
#define UI_H

#include <boost/shared_ptr.hpp>
#include <qobject.h>
#include "common.h"
#include "controllers/entitytoken.h"
#include "tagua.h"
#include <map>

class Controller;
class Engine;
class ActionCollection;
class KActionCollection;
class ActionStateObserver;

/**
  * @brief Utility class to handle GUI actions.
  * 
  * An instance of UI is owned by MainWindow, and every GUI action
  * is connected to a slot of UI.
  * A UI instance maintains a correspondence between Tagua tabs and
  * associated controllers. Besides, UI knows the active tab (hence
  * controller) and directs all user interaction to it.
  * Whenever a new tab is created, the corresponding controller is
  * added to the association, and its setUI member function is called,
  * so that it can setup all the observers.
  * 
  * To add a new tab <-> controller association, use the addController
  * member function. 
  * If you want to replace the current controller, use setController.
  * Both functions assume that the controller is new, hence initialize
  * it via setUI.
  */
class UI : public QObject {
Q_OBJECT
  typedef std::map<QWidget*, boost::shared_ptr<Controller> > ControllerMap;
  ControllerMap m_controller;
  QWidget* m_current_tab;
  boost::shared_ptr<Controller>& controller();
  boost::shared_ptr<Controller> controller() const;
  KActionCollection* m_actions;
  friend class UIActionStateObserver;
public:
  /**
    * Constructor.
    * \param actions The action collection associated to this UI instance. 
    *                This will be used to enable/disable actions according
    *                to notifications coming from the controller.
    */
  UI(KActionCollection* actions);
  
  /**
    * Add a new tab <-> controller association.
    * The controller is assumed to be new, and it is initialized by calling
    * its setUI member function.
    */
  void addController(QWidget* tab, const boost::shared_ptr<Controller>&);
  /**
    * Just like addController, but replace the current controller instead
    * of creating a new association. The old controller is no longer referenced
    * by UI, so it will be destroyed, unless other objects keep it alive.
    */
  void setController(const boost::shared_ptr<Controller>& controller);
  /**
    * Remove a controller. The controller will no longer be referenced by
    * UI, so it will be destroyed, unless other objects keep it alive.
    */
  void removeController(QWidget* tab);
  
  /**
    * Create an action state observer that reacts to action state
    * updates by enabling / disabling the corresponding actions.
    * \sa ActionStateObserver
    */
  boost::shared_ptr<ActionStateObserver> 
    createActionStateObserver(const boost::shared_ptr<Controller>&) const;
public Q_SLOTS:
  /**
    * Change the current tab and associated controller.
    */
  void setCurrentTab(QWidget* tab);

  bool undo();
  bool redo();
  bool truncate();
  bool promoteVariation();
  bool back();
  bool forward();
  void gotoFirst();
  void gotoLast();
  void setTurn(int);
  void createCtrlAction();
  void destroyCtrlAction();
  ActionCollection* variantActions() const;

  void pgnCopy();
  void pgnPaste();
  void pgnPaste(const QString&);
  QString currentPGN();
  
  // editing
  void clearBoard();
  void setStartingPosition();
  void copyPosition();
  void pastePosition();
  AbstractPosition::Ptr position() const;

  EntityToken addPlayingEngine(int side, const boost::shared_ptr<Engine>& engine);
//   EntityToken addAnalysingEngine(const boost::shared_ptr<Engine>& engine);
  void removeEntity(const EntityToken& token);

  void end();
  void detach();
  
  QString currentVariant() const;
  void reloadSettings();
};

#endif
