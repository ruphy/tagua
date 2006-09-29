/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include <QApplication>
#include "settings.h"
#include "pref_board.h"


PrefBoard::PrefBoard(QWidget *parent)
: QWidget(parent) {

  setupUi(this);

  groupAnimations->setChecked((settings["AnimationsEnabled"]|=true).value<bool>());
  checkMovements->setChecked((settings["AnimateMovement"]|=true).value<bool>());
  checkExplosions->setChecked((settings["AnimateExplode"]|=true).value<bool>());
  checkFading->setChecked((settings["AnimateFade"]|=true).value<bool>());
  checkTransformations->setChecked((settings["AnimateTransform"]|=true).value<bool>());
  checkSequence->setChecked((settings["AnimationsSequence"]|=true).value<bool>());
  spinSequenceMax->setValue((settings["AnimationsSequenceMax"]|=10).value<int>());

  sliderSpeed->setValue((settings["AnimationsSpeed"]|=16).value<int>());
  sliderSmoothness->setValue((settings["AnimationsSmoothness"]|=16).value<int>());

  groupBorder->setChecked((settings["BoardBorderShow"]|=true).value<bool>());
  colorBorder->setColor((settings["BoardBorderColor"]|=QColor(Qt::white)).value<QColor>());
  colorBorderText->setColor((settings["BoardBorderTextColor"]|=QColor(Qt::black)).value<QColor>());
  fontBorder->setFont((settings["BoardBorderFont"]|=QApplication::font()).value<QFont>());
}

PrefBoard::~PrefBoard() {
}

void PrefBoard::apply() {
  settings["AnimationsEnabled"] = groupAnimations->isChecked();
  settings["AnimateMovement"] = checkMovements->isChecked();
  settings["AnimateExplode"] = checkExplosions->isChecked();
  settings["AnimateFade"] = checkFading->isChecked();
  settings["AnimateTransform"] = checkTransformations->isChecked();
  settings["AnimationsSequence"] = checkSequence->isChecked();
  settings["AnimationsSequenceMax"] = spinSequenceMax->value();

  settings["AnimationsSpeed"] = sliderSpeed->value();
  settings["AnimationsSmoothness"] = sliderSmoothness->value();

  settings["BoardBorderShow"] = groupBorder->isChecked();
  settings["BoardBorderColor"] = colorBorder->color();
  settings["BoardBorderTextColor"] = colorBorderText->color();
  settings["BoardBorderFont"] = fontBorder->font();
}

#include "pref_board.moc"
