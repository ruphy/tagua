/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
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
#include "kboard.h"
#include <map>

class GenericPosition;
class Controller;
class PositionInfo;
class PGN;
class Engine;

class UI : public QObject {
Q_OBJECT
  typedef std::map<QWidget*, boost::shared_ptr<Controller> > ControllerMap;
  ControllerMap m_controller;
  QWidget* m_current_tab;
  boost::shared_ptr<Controller>& controller();
  const boost::shared_ptr<Controller>& controller() const;
public:
  UI();
  void addController(QWidget* w, const boost::shared_ptr<Controller>&);
  void setController(const boost::shared_ptr<Controller>&);
  void removeController(QWidget* w);
public slots:
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

  void pgnCopy();
  void pgnPaste();
  void pgnPaste(const QString&);

  // promotion type
  void promoteToQueen();
  void promoteToRook();
  void promoteToBishop();
  void promoteToKnight();
  int promotionType() const;
  
  // do promotion
  void setDoPromotion(bool);
  bool doPromotion() const;

  // editing
  void clearBoard();
  void setStartingPosition();
  void copyPosition();
  void pastePosition();
  AbstractPosition::Ptr position() const;

  void addPlayingEngine(int side, const boost::shared_ptr<Engine>& engine);
  EntityToken addAnalysingEngine(const boost::shared_ptr<Engine>& engine);
  void removeAnalysingEngine(const EntityToken& token);

  void end();
  void detach();
};

#endif
