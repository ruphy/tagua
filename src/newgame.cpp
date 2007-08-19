/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "newgame.h"
#include "foreach.h"
#include "variants.h"

NewGame::NewGame(QWidget* parent)
: QDialog(parent) {
  setupUi(this);
  QStringList variants = Variants::instance().all();
  foreach (QString variant, variants) {
    m_variant->addItem(variant, QVariant(variant));
  }
}

QString NewGame::variant() const {
  return m_variant->itemData(m_variant->currentIndex()).toString();
}

bool NewGame::newTab() const {
  return m_new_tab->isChecked();
}
