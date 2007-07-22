/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREFERENCES__MOVELIST_H
#define PREFERENCES__MOVELIST_H

#include "ui_pref_movelist.h"

namespace MoveList { class Settings; }

class PrefMoveList : public QWidget
                   , private Ui::PrefMoveList {
Q_OBJECT

  MoveList::Settings *m_settings;

public:
  PrefMoveList(QWidget *parent = 0);
  ~PrefMoveList();

public Q_SLOTS:
  void apply();
};

#endif //PREFERENCES__MOVELIST_H
