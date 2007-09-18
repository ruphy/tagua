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
  UI(KActionCollection* actions);
  void addController(QWidget* w, const boost::shared_ptr<Controller>&);
  void setController(const boost::shared_ptr<Controller>&);
  void removeController(QWidget* w);
  
  boost::shared_ptr<ActionStateObserver> 
    createActionStateObserver(const boost::shared_ptr<Controller>&) const;
public Q_SLOTS:
  void setCurrentTab(QWidget* w);

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
