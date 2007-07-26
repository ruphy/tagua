/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREFERENCES__BOARD_H
#define PREFERENCES__BOARD_H

#include "ui_pref_board.h"

class PrefBoard : public QWidget
                , private Ui::PrefBoard {
Q_OBJECT

public:
  PrefBoard(const QString& currentVariant, QWidget *parent = 0);
  ~PrefBoard();

public Q_SLOTS:
  void apply();
};

#endif //PREFERENCES__BOARD_H
