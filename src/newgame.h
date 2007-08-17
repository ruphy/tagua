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

#include "ui_newgamedialog.h"
#include "option.h"

class QHBoxLayout;

class NewGame : public QDialog, public Ui::NewGameDialog {
Q_OBJECT
  bool m_allow_current;
  OptList m_custom_options;
  OptionWidget *m_custom_opt_widget;
  QHBoxLayout *m_custom_opt_layout;
public:
  NewGame(QWidget* parent = 0, bool allowCurrent = false);
  QString variant() const;
  bool playFromCurrent() const;
  bool isCustom() const;
  OptList customOptions() { return m_custom_options; }
private Q_SLOTS:
  void variantChanged(const QString&);
};

#endif

