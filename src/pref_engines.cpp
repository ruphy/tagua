/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "pref_engines.h"
#include "settings.h"

PrefEngines::PrefEngines(QWidget* parent)
: QWidget(parent) {
  setupUi(this);

  m_add_engine->setIcon(QIcon(data_dir() + "/icons/add.png"));
  m_remove_engine->setIcon(QIcon(data_dir() + "/icons/remove.png"));
}

PrefEngines::~PrefEngines() { }

void PrefEngines::apply() {

}

#include "pref_engines.moc"
