/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <qapplication.h>
#include <qclipboard.h>
#include "ui.h"
#include "controllers/abstract.h"
#include "variants/xchess/piecetype.h"

using namespace boost;

UI::UI()
: m_current_tab(NULL) {
}

boost::shared_ptr<Controller>& UI::controller() {
  Q_ASSERT(m_current_tab);

  return m_controller[m_current_tab];
}

const boost::shared_ptr<Controller>& UI::controller() const {
  Q_ASSERT(m_current_tab);

  return m_controller.find(m_current_tab)->second;
}

void UI::addController(QWidget* w, const shared_ptr<Controller>& controller) {
  m_controller[w] = controller;
}

void UI::setController(const shared_ptr<Controller>& controller) {
  Q_ASSERT(m_current_tab);

  m_controller[m_current_tab] = controller;
}

void UI::removeController(QWidget* w) {
  controller()->end();
  m_controller.erase(w);
}

void UI::setCurrentTab(QWidget* w) {
  m_current_tab = w;
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
  cb->setText(controller()->save());
}

void UI::pgnPaste() {
  QClipboard* cb = QApplication::clipboard();
  pgnPaste(cb->text());
}

void UI::pgnPaste(const QString& pgn) {
  //controller()->loadPGN(pgn);
}

void UI::promoteToQueen() {
  controller()->setPromotionType(QUEEN);
}

void UI::promoteToRook() {
  controller()->setPromotionType(ROOK);
}

void UI::promoteToBishop() {
  controller()->setPromotionType(BISHOP);
}

void UI::promoteToKnight() {
  controller()->setPromotionType(KNIGHT);
}

int UI::promotionType() const {
  return controller()->promotionType();
}

bool UI::doPromotion() const {
  return controller()->doPromotion();
}

void UI::setDoPromotion(bool value) {
  controller()->setDoPromotion(value);
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

void UI::addPlayingEngine(int side, const shared_ptr<Engine>& engine) {
  controller()->addPlayingEngine(side, engine);
}

EntityToken UI::addAnalysingEngine(const shared_ptr<Engine>& engine) {
  return controller()->addAnalysingEngine(engine);
}

void UI::removeAnalysingEngine(const EntityToken& token) {
  controller()->removeAnalysingEngine(token);
}

void UI::end() {
  controller() = controller()->end();
}

void UI::detach() {
  controller()->detach();
}

#include "ui.moc"
