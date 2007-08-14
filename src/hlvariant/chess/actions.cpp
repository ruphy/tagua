/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "actions.h"
#include "../variantdata.h"
#include "variant.h"

#include <QActionGroup>

#include <KAction>
#include <KIcon>

QAction* createAction(QActionGroup* group, const KIcon& icon, const QString& text) {
  QAction* action = new KAction(icon, text, group);
  action->setCheckable(true);
  group->addAction(action);
  return action;
}

namespace HLVariant {
namespace Chess {

Actions::Actions()
: m_promotion(VariantData<Variant>::Piece::QUEEN) {
  QActionGroup* group = new QActionGroup(this);
  
  QAction* queen = createAction(group, KIcon("promoteQueen"), "Promote to queen");
  queen->setChecked(true);
  add(queen);
  connect(queen, SIGNAL(triggered()), this, SLOT(promote_to_queen()));
  
  QAction* rook = createAction(group, KIcon("promoteRook"), "Promote to rook");
  add(rook);
  connect(rook, SIGNAL(triggered()), this, SLOT(promote_to_rook()));
  
  QAction* bishop = createAction(group, KIcon("promoteBishop"), "Promote to bishop");
  add(bishop);
  connect(bishop, SIGNAL(triggered()), this, SLOT(promote_to_bishop()));
  
  QAction* knight = createAction(group, KIcon("promoteKnight"), "Promote to knight");
  add(knight);
  connect(knight, SIGNAL(triggered()), this, SLOT(promote_to_knight()));
}

int Actions::promotion() const {
  return m_promotion;
}

void Actions::promote_to_queen() {
  m_promotion = VariantData<Variant>::Piece::QUEEN;
}

void Actions::promote_to_rook() {
  m_promotion = VariantData<Variant>::Piece::ROOK;
}

void Actions::promote_to_bishop() {
  m_promotion = VariantData<Variant>::Piece::BISHOP;
}

void Actions::promote_to_knight() {
  m_promotion = VariantData<Variant>::Piece::KNIGHT;
}

}
}

