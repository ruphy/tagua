/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
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
#include <vector>
#include <map>


class PrefEngines : public QWidget
                  , public Ui::PrefEngines {
Q_OBJECT
  std::map<QListWidgetItem*, EngineDetails> m_engines;
  
  EngineDetails& currentEngine();
public:
  PrefEngines(QWidget* parent = 0);
  ~PrefEngines();
  
  QListWidgetItem* addEngine(const EngineDetails& details);
public slots:
  void apply();
  void setName(QListWidgetItem*);
  void add();
  void remove();
  void changeEngine(QListWidgetItem* item);
  
  void setPath();
  void setType();
  void setWorkDir();
};

#endif // PREF_ENGINES_H

