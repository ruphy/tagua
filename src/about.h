/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ABOUT_H
#define ABOUT_H

#include "ui/ui_about.h"

class About : public QDialog,
              private Ui::About {
Q_OBJECT

public:
  About(QWidget *parent = 0);
};

#endif //ABOUT_H
