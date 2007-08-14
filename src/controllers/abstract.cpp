/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "abstract.h"
#include "entities/userentity.h"

Controller::Controller(ChessTable* view)
: m_view(view) { }

Controller::~Controller() { }

ActionCollection* Controller::variantActions() const {
  return m_variant_actions;
}

bool Controller::undo() {
  return entity()->undo();
}

bool Controller::redo() {
  return entity()->redo();
}

bool Controller::truncate() {
  return entity()->truncate();
}

bool Controller::promoteVariation() {
  return entity()->promoteVariation();
}

void Controller::setPromotionType(int p) {
  entity()->changePromotionType(p);
}

int Controller::promotionType() const {
  return entity()->promotionType();
}

bool Controller::doPromotion() const {
  return entity()->promotionType() >= 0;
}

void Controller::setDoPromotion(bool value) {
  return entity()->changePromotionType(value ? 0 : -1);
}

bool Controller::back() {
  return entity()->back();
}

bool Controller::forward() {
  return entity()->forward();
}

void Controller::gotoFirst() {
  entity()->gotoFirst();
}

void Controller::gotoLast() {
  entity()->gotoLast();
}

QString Controller::save() {
  return entity()->save();
}

void Controller::loadPGN(const PGN& pgn) {
  entity()->loadPGN(pgn);
}

