/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QApplication>
#include <QClipboard>
#include <KActionCollection>
#include <KAction>
#include "ui.h"
#include "controllers/abstract.h"
#include "graphicalgame.h"

using namespace boost;

#define SYNC_ACTION(NAME, ENUM) \
  m_ui->m_actions->action(NAME)->setEnabled(state & GraphicalGame::ENUM)
class UIActionStateObserver : public ActionStateObserver {
  const UI* m_ui;
  Controller* m_controller;
public:
  UIActionStateObserver(const UI* ui, const shared_ptr<Controller>& controller)
  : m_ui(ui)
  , m_controller(controller.get()) { }
  
  virtual void notifyActionStateChange(GraphicalGame::ActionState state) {
    // check that the associated controller is active
    if (m_ui->controller().get() == m_controller) {
      SYNC_ACTION("begin", BEGIN);
      SYNC_ACTION("back", BACK);
      SYNC_ACTION("forward", FORWARD);
      SYNC_ACTION("end", END);
      SYNC_ACTION("edit_undo", UNDO);
      SYNC_ACTION("edit_redo", REDO);
    }
  }
};
#undef SYNC_ACTION

UI::UI(KActionCollection* actions)
: m_current_tab(NULL)
, m_actions(actions) {
}

boost::shared_ptr<Controller>& UI::controller() {
  Q_ASSERT(m_current_tab);

  boost::shared_ptr<Controller>& res = m_controller[m_current_tab];
  Q_ASSERT(res);
  
  return res;
}

boost::shared_ptr<Controller> UI::controller() const {
  Q_ASSERT(m_current_tab);

  ControllerMap::const_iterator it = m_controller.find(m_current_tab);
  Q_ASSERT(it != m_controller.end());
  return it->second;
}

void UI::addController(QWidget* w, const shared_ptr<Controller>& controller) {
  m_current_tab = w;
  setController(controller);
}

void UI::setController(const shared_ptr<Controller>& controller) {
  Q_ASSERT(m_current_tab);
  m_controller[m_current_tab] = controller;
  controller->setUI(*this);
}

void UI::removeController(QWidget* w) {
  controller()->end();
  std::cout << "removing controller " << w << std::endl;
  m_controller.erase(w);
}

void UI::setCurrentTab(QWidget* w) {
  m_current_tab = w;
  controller()->activate();
}

bool UI::undo() {
  controller()->undo();
  return true;
}

bool UI::redo() {
  controller()->redo();
  return true;
}

bool UI::truncate() {
  controller()->truncate();
  return true;
}

bool UI::promoteVariation() {
  controller()->promoteVariation();
  return true;
}

bool UI::back() {
  return controller()->back();
}

bool UI::forward() {
  return controller()->forward();
}

void UI::gotoFirst() {
  controller()->gotoFirst();
}

void UI::gotoLast() {
  controller()->gotoLast();
}

void UI::pgnCopy() {
  QClipboard* cb = QApplication::clipboard();
  cb->setText(currentPGN());
}

QString UI::currentPGN() {
  return controller()->save();
}

void UI::pgnPaste() {
  QClipboard* cb = QApplication::clipboard();
  pgnPaste(cb->text());
}

void UI::pgnPaste(const QString&) {
  //controller()->loadPGN(pgn);
}

void UI::clearBoard() {
  controller()->clearBoard();
}

void UI::setStartingPosition() {
  controller()->setStartingPosition();
}

void UI::copyPosition() {
  QClipboard* cb = QApplication::clipboard();
  cb->setText(controller()->fen());
}

void UI::pastePosition() {
  QClipboard* cb = QApplication::clipboard();
  controller()->setFEN(cb->text());
}

void UI::setTurn(int turn) {
  controller()->setTurn(turn);
}

AbstractPosition::Ptr UI::position() const {
  return controller()->currentPosition();
}

void UI::createCtrlAction() {
  controller()->createCtrlAction();
}

void UI::destroyCtrlAction() {
  controller()->destroyCtrlAction();
}

ActionCollection* UI::variantActions() const {
  return controller()->variantActions();
}

EntityToken UI::addPlayingEngine(int side, const shared_ptr<Engine>& engine) {
  return controller()->addPlayingEngine(side, engine);
}

// EntityToken UI::addAnalysingEngine(const shared_ptr<Engine>& engine) {
//   return controller()->addAnalysingEngine(engine);
// }

void UI::removeEntity(const EntityToken& token) {
  controller()->removeEntity(token);
}

void UI::end() {
  controller() = controller()->end();
}

void UI::detach() {
  controller()->detach();
}

QString UI::currentVariant() const {
  return controller()->variant();
}

void UI::reloadSettings() {
  for (ControllerMap::iterator it = m_controller.begin(),
       end = m_controller.end();
       it != end;
       ++it) {
    it->second->reloadSettings();
  }
}

shared_ptr<ActionStateObserver>
UI::createActionStateObserver(const shared_ptr<Controller>& controller) const {
  return shared_ptr<UIActionStateObserver>(
    new UIActionStateObserver(this, controller));
}
