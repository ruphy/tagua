/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "about.h"
#include "settings.h"
#include <QPalette>


void setBackgroundColor(QWidget* w) {
  QPalette palette = w->palette();
  palette.setColor(QPalette::Base,
    palette.color(QPalette::Window));
  w->setPalette(palette);
}


About::About(QWidget *parent)
: QDialog(parent) {

  setupUi(this);
  setWindowIcon(QIcon(data_dir()+"/icons/kboard.svg"));
  setBackgroundColor(m_authors);
  setBackgroundColor(m_thanks);
  setBackgroundColor(m_licence);
}


#include "about.moc"
