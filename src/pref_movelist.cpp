/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include "settings.h"
#include "movelist_p.h"
#include "pref_movelist.h"


PrefMoveList::PrefMoveList(QWidget *parent)
: QWidget(parent) {

  setupUi(this);

  m_settings = new MoveList::Settings;
  m_settings->load();

  groupAnimations->setChecked(m_settings->anim_enabled);
  checkFading->setChecked(m_settings->anim_hideshow);
  checkHighlighting->setChecked(m_settings->anim_highlight);
  checkMovements->setChecked(m_settings->anim_moving);
  sliderSpeed->setValue(m_settings->anim_speed);
  sliderSmoothness->setValue(m_settings->anim_smoothness);

  colorSelected->setColor(m_settings->select_color);
  colorComments->setColor(m_settings->comment_color);
  checkMovesFont->setChecked(m_settings->use_mv_font);
  fontMoves->setFont(m_settings->mv_font);
  checkCommentsFont->setChecked(m_settings->use_comm_font);
  fontComments->setFont(m_settings->comm_font);
}

PrefMoveList::~PrefMoveList() {
  delete m_settings;
}

void PrefMoveList::apply() {
  m_settings->anim_enabled   = groupAnimations->isChecked();
  m_settings->anim_hideshow  = checkFading->isChecked();
  m_settings->anim_highlight = checkHighlighting->isChecked();
  m_settings->anim_moving    = checkMovements->isChecked();
  m_settings->anim_speed     = sliderSpeed->value();
  m_settings->anim_smoothness = sliderSmoothness->value();

  m_settings->select_color  = colorSelected->color();
  m_settings->comment_color = colorComments->color();
  m_settings->use_mv_font   = checkMovesFont->isChecked();
  m_settings->mv_font       = fontMoves->font();
  m_settings->use_comm_font = checkCommentsFont->isChecked();
  m_settings->comm_font     = fontComments->font();

  m_settings->save();
}

