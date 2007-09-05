/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREF_ENGINES_H
#define PREF_ENGINES_H

#include "ui_pref_engines.h"
#include "engineinfo.h"
#include "settings.h"
#include <vector>
#include <map>

class QPushButton;
class QListWidget;

class PrefEngines : public QWidget {
Q_OBJECT
  QPushButton* m_new;
  QPushButton* m_edit;
  QPushButton* m_delete;
  QListWidget* m_list;
  
  QList<EngineDetails> m_engines;
  void reload_engines();
public:
  PrefEngines(const QString&, QWidget* parent = 0);
  ~PrefEngines();

  void apply();
private Q_SLOTS:
  void new_engine();
  void edit_engine();
  void delete_engine();
  void update_buttons();
};

#endif // PREF_ENGINES_H

