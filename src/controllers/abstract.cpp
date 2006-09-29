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

void Controller::loadPGN(const QString& pgn) {
  entity()->loadPGN(pgn);
}

