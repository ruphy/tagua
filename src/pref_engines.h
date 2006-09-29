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

#include "ui/ui_pref_engines.h"
#include <vector>

class EngineInfo;

class PrefEngines : public QWidget
                  , public Ui::PrefEngines {
Q_OBJECT
  std::vector<EngineInfo*> m_engine_info;
public:
  PrefEngines(QWidget* parent = 0);
  ~PrefEngines();
public slots:
  void apply();
};

#endif // PREF_ENGINES_H

