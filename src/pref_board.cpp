/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include <QApplication>
#include "global.h"
#include "pref_board.h"


PrefBoard::PrefBoard(QWidget *parent)
: QWidget(parent) {

  setupUi(this);

  {
    Settings s_anim = settings.group("animations");
    groupAnimations->setChecked(s_anim.flag("enabled", true));
    checkMovements->setChecked(s_anim["movement"].flag("enabled", true));
    checkExplosions->setChecked(s_anim["explode"].flag("enabled", true));
    checkFading->setChecked(s_anim["fading"].flag("enabled", true));
    checkTransformations->setChecked(s_anim["transform"].flag("enabled", true));
    {
      Settings s_sequence = s_anim.group("sequence");
      checkSequence->setChecked(s_sequence.flag("enabled", true));
      spinSequenceMax->setValue(s_sequence["max"] | 10);
    }

    sliderSpeed->setValue(s_anim["speed"] | 16);
    sliderSmoothness->setValue(s_anim["smoothness"] | 16);
  }

  {
    Settings s_border = settings.group("board-border");
    groupBorder->setChecked(s_border.flag("visible", "true"));
    colorBorder->setColor(s_border["color"] |= QColor(Qt::white));
    colorBorderText->setColor(s_border["text-color"] |= QColor(Qt::black));
    fontBorder->setFont(s_border["font"] |= QApplication::font());
  }
}

PrefBoard::~PrefBoard() {
}

void PrefBoard::apply() {
  {
    Settings s_anim = settings.group("animations");
    s_anim.setFlag("enabled", groupAnimations->isChecked());
    s_anim.group("movement").setFlag("enabled", checkMovements->isChecked());
    s_anim.group("explode").setFlag("enabled", checkExplosions->isChecked());
    s_anim.group("fading").setFlag("enabled", checkFading->isChecked());
    s_anim.group("transform").setFlag("enabled", checkTransformations->isChecked());
    {
      Settings s_sequence = s_anim.group("sequence");
      s_sequence.setFlag("enabled", checkSequence->isChecked());
      s_sequence["max"] = spinSequenceMax->value();
    }

    s_anim["speed"] = sliderSpeed->value();
    s_anim["smoothness"] = sliderSmoothness->value();
  }

  {
    Settings s_border = settings.group("board-border");
    s_border.setFlag("visible", groupBorder->isChecked());
    s_border["color"] = colorBorder->color();
    s_border["text-color"] = colorBorderText->color();
    s_border["font"] = fontBorder->font();
  }
}

#include "pref_board.moc"
