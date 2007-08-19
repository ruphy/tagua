/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef NEWGAME_H
#define NEWGAME_H

#include <QDialog>
#include "ui_newgamedialog.h"

class NewGame : public QDialog, public Ui::NewGameDialog {
Q_OBJECT
public:
  NewGame(QWidget* parent = 0);
  QString variant() const;
  bool newTab() const;
};

#endif

