/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "clock.h"
#include "constrainedtext.h"
#include <math.h>
#include <iostream>

Clock::Clock(int col, KGameCanvasAbstract* canvas)
: ClickableCanvas(canvas)
, m_color(col)
, m_running(false) {
  m_background   = new KGameCanvasPixmap(this);
  m_caption      = new ConstrainedText(this);
  m_time_label   = new ConstrainedText(this);
  m_player_name  = new ConstrainedText(this);
  m_decs         = new ConstrainedText(this);

  m_background->show();
  m_caption->show();
  m_time_label->show();
  m_player_name->show();

  setTime(0);
  setPlayer(Player());
  m_caption->setText(col == 0 ? "White" : "Black");
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

Clock::~Clock() {
  delete m_background;
  delete m_caption;
  delete m_time_label;
  delete m_player_name;
  delete m_decs;
}

void Clock::start() {
  m_running = true;
  m_time.start();
  m_timer.start(100);
}

void Clock::stop() {
  if (m_running) m_total_time -= m_time.elapsed();
  m_running = false;
  m_timer.stop();
}

void Clock::activate(bool a) {
  if(m_active == a)
    return;

  m_active = a;
  m_background->setPixmap(m_active ? m_active_pixmap : m_inactive_pixmap);

  m_time_label->setColor(m_active ? m_active_text : m_inactive_text);
  m_player_name->setColor(m_active ? m_active_text : m_inactive_text);
  m_caption->setColor(m_active ? m_active_text : m_inactive_text);
}

void Clock::tick() {
  computeTime();
}

void Clock::computeTime() {
  int time = m_total_time;
  if (m_running) time -= m_time.elapsed();

  bool positive;
  int total_secs;
  int decs = -1;

  if (time > 0 && time < 10000) {
    int total_decs = static_cast<int>(ceil(time / 100.0));
    positive = total_decs >= 0;
    if (!positive) total_decs = -total_decs;
    decs = total_decs % 10;
    total_secs = total_decs / 10;
  }
  else {
    total_secs = static_cast<int>(ceil(time / 1000.0));
    positive = total_secs >= 0;
    if (!positive) total_secs = -total_secs;
  }


  int secs = total_secs % 60;
  int mins = total_secs / 60;
  QString timeText;

  {
    QString secText = QString::number(secs);
    if (secText.length() < 2) secText = "0" + secText;

    QString minText = QString::number(mins);
    if (minText.length() < 2) minText = "0" + minText;

    timeText = minText + ":" + secText;
    if (!positive)
      timeText = "-" + timeText;

#if 0
    if (positive && decs != -1) {
      int dec = static_cast<int>(ceil(time / 100.0)) % 10;

      m_decs->moveTo(m_time_label->rect().bottomRight() + QPoint(2, 0));
      m_decs->setText(":" + QString::number(dec));
      m_decs->show();
    }
    else
      m_decs->hide();
#endif
  }

  m_time_label->setText(timeText);
}

QString Clock::playerString(const Player& player) {
  QString rating = player.rating != -1 ? QString(" (%1)").arg(player.rating) : QString();
  return QString("%1").arg(player.name) + rating;
}

void Clock::setPlayer(const Player& player) {
  m_player_name->setText(playerString(player));
}

void Clock::setTime(int t) {
  m_total_time = t;
  tick();
}

void Clock::onMousePress(const QPoint& /*pos*/, int /*button*/) {
}

void Clock::resize(int size) {
  m_controls_loader.setSize(size);

  m_height = (int)m_controls_loader.getValue<double>("clock_height");

  m_active_pixmap = m_controls_loader.getValue<QPixmap>("clock_active_background");
  m_inactive_pixmap = m_controls_loader.getValue<QPixmap>("clock_inactive_background");

  m_active_text = m_controls_loader.getValue<QBrush>("clock_active_text").color();
  m_inactive_text = m_controls_loader.getValue<QBrush>("clock_inactive_text").color();

  m_background->setPixmap(m_active ? m_active_pixmap : m_inactive_pixmap);
  m_background->moveTo(m_controls_loader.getValue<QPointF>("clock_background_offset").toPoint());

  m_time_label->setConstrainRect(m_controls_loader.getValue<QRectF>("clock_time_rect").toRect());
  m_time_label->setColor(m_active ? m_active_text : m_inactive_text);

  m_player_name->setConstrainRect(m_controls_loader.getValue<QRectF>("clock_player_rect").toRect());
  m_player_name->setColor(m_active ? m_active_text : m_inactive_text);

  m_caption->setConstrainRect(m_controls_loader.getValue<QRectF>("clock_caption_rect").toRect());
  m_caption->setColor(m_active ? m_active_text : m_inactive_text);
}

void Clock::settingsChanged() {
  m_caption->setFont(m_controls_loader.getStaticValue<QFont>("clock_caption_font"));
  m_player_name->setFont(m_controls_loader.getStaticValue<QFont>("clock_player_font"));
  m_time_label->setFont(m_controls_loader.getStaticValue<QFont>("clock_time_font"));
}

