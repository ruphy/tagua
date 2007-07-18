/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "pref_clock.h"
#include "clock.h"
#include "mastersettings.h"

#include <QBoxLayout>
#include <QSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QColorDialog>

#include <iostream>

PrefClock::PrefClock() {
  QVBoxLayout* main_layout = new QVBoxLayout(this);

  {
    QHBoxLayout* layout = new QHBoxLayout;
    QLabel* label = new QLabel("Background color:", this);
    m_background_button = new QPushButton("", this);
    layout->addWidget(label);
    layout->addStretch();
    layout->addWidget(m_background_button);
    main_layout->addLayout(layout);
    connect(m_background_button, SIGNAL(clicked()), this, SLOT(setBackgroundColor()));
  }

  createSpin(main_layout, "Caption font size:", m_captionSpin);
  createSpin(main_layout, "Time font size:", m_timeSpin);
  createSpin(main_layout, "Decimals font size:", m_decsSpin);
  createSpin(main_layout, "Player font size:", m_playerSpin);

  QHBoxLayout* button_layout = new QHBoxLayout;
  button_layout->addStretch();
  m_ok = new QPushButton("&OK", this);
  m_cancel = new QPushButton("&Cancel", this);
  button_layout->addWidget(m_ok);
  button_layout->addWidget(m_cancel);
  connect(m_ok, SIGNAL(clicked()), this, SLOT(accept()));
  connect(m_cancel, SIGNAL(clicked()), this, SLOT(reject()));

  main_layout->addLayout(button_layout);

  load();
}

void PrefClock::load() {
  Settings s = settings.group("Clock");
//  std::cout << "caption font size: " << settings["captionFontSize"].value<int>() << std::endl;
  m_captionSpin->setValue(s["captionFontSize"].value<int>());
  m_timeSpin->setValue(s["timeFontSize"].value<int>());
  m_decsSpin->setValue(s["decsFontSize"].value<int>());
  m_playerSpin->setValue(s["playerFontSize"].value<int>());

  {
    QPalette palette = m_background_button->palette();
    palette.setColor(QPalette::Button, s["background"].value<QColor>());
    m_background_button->setPalette(palette);
  }
}

void PrefClock::createSpin(QBoxLayout* parent, const QString& labelText, QSpinBox*& spin) {
  QHBoxLayout* layout = new QHBoxLayout;
  parent->addLayout(layout);
  spin = new QSpinBox(this);
  spin->setRange(6, 40);
  QLabel* label = new QLabel(labelText, this);
  layout->addWidget(label);
  layout->addStretch();
  layout->addWidget(spin);
}

void PrefClock::apply() {
  Settings s = settings.group("Clock");
  s["captionFontSize"] = m_captionSpin->value();
  s["timeFontSize"] = m_timeSpin->value();
  s["decsFontSize"] = m_decsSpin->value();
  s["playerFontSize"] = m_playerSpin->value();
  s["background"] = m_background_button->palette().color(QPalette::Button);
}

void PrefClock::setBackgroundColor() {
  QColor color = QColorDialog::getColor(m_background_button->palette().color(QPalette::Button));
  if (color.isValid()) {
    QPalette palette = m_background_button->palette();
    palette.setColor(QPalette::Button, color);
    m_background_button->setPalette(palette);
  }
}
