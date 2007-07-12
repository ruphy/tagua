/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QApplication>
#include <QHBoxLayout>
#include <kicon.h>
#include "global.h"
#include "pref_board.h"
#include "pref_movelist.h"
#include "pref_theme.h"
#include "pref_engines.h"
#include "pref_preferences.h"

template<typename T>
class PrefWrapper : public PrefBase {
private:
  T *m_inner;

public:
  PrefWrapper(QWidget *parent = 0)
    : PrefBase(parent)
    , m_inner(NULL) {
  }

  virtual void apply() {
    if(m_inner)
      m_inner->apply();
  }

  virtual void showEvent( QShowEvent * /*event*/ ) {
    if(m_inner)
      return;

    m_inner = new T(this);
    QHBoxLayout *l = new QHBoxLayout(this);
    l->addWidget(m_inner);
    m_inner->show();
  }
};


Preferences::Preferences(QWidget *parent)
: QDialog(parent) {

  setupUi(this);
  setWindowIcon(KIcon("kboard"));
  connect(buttonBox->button(QDialogButtonBox::Apply), SIGNAL(clicked()), this, SLOT(apply()));

  pagePref->hide();
  PrefBase *b;
  KPageWidgetItem *i;

  b = new PrefWrapper<PrefBoard>(this);
  connect(this, SIGNAL(applied()), b, SLOT(apply()));
  i = pagePref->addPage(b, "Board");
  i->setHeader("Board preferences:");
  i->setIcon(KIcon("prefBoard"));

  b = new PrefWrapper<PrefMoveList>(this);
  connect(this, SIGNAL(applied()), b, SLOT(apply()));
  i = pagePref->addPage(b, "Move list");
  i->setHeader("Move list preferences:");
  i->setIcon(KIcon("prefMoveList"));

  b = new PrefWrapper<PrefTheme>(this);
  connect(this, SIGNAL(applied()), b, SLOT(apply()));
  i = pagePref->addPage(b, "Theme");
  i->setHeader("Pieces & squares theme:");
  i->setIcon(KIcon("prefTheme"));
  pagePref->show();

  b = new PrefWrapper<PrefEngines>(this);
  connect(this, SIGNAL(applied()), b, SLOT(apply()));
  i = pagePref->addPage(b, "Engines");
  i->setHeader("Engines:");
  i->setIcon(KIcon("brain"));
  pagePref->show();
}

Preferences::~Preferences() {
}

void Preferences::apply() {
  emit applied();
  settings.changed();
}

