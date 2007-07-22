/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREFERENCES__PREFERENCES_H
#define PREFERENCES__PREFERENCES_H

#include <list>
#include "ui_preferences.h"


class PrefBase : public QWidget {
Q_OBJECT

public:
  PrefBase(QWidget *parent = 0) : QWidget(parent) {}

public Q_SLOTS:
  virtual void apply() = 0;
};


class Preferences : public QDialog
                  , private Ui::Preferences {
Q_OBJECT

  std::list<PrefBase*> m_pages;

public:
  Preferences(QWidget *parent = 0);
  ~Preferences();

Q_SIGNALS:
  void applied();

public Q_SLOTS:
  void apply();
};

#endif //PREFERENCES__PREFERENCES_H
