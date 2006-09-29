/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <math.h>
#include <iostream>
#include <QResizeEvent>
#include "clock.h"
#include "settings.h"

static void setFontSize(int max, int width, const QString& text, QFont& font) {
  font.setPointSize(max);
  return; // FIXME
  while (max >= 8) {
    QTime tm; tm.start();
    QFontMetrics metrics(font);
    int fw = metrics.boundingRect(text).width();
    std::cout << "font metrics: " << tm.elapsed() << std::endl;

    if (fw <= width) break;
    max--;
    font.setPointSize(max);
  }
}

void Clock::Info::settingsChanged() {
}

void Clock::Info::setup(const Player& player, const QRect& rect, const QString& caption, Canvas::Abstract* canvas) {
  putInCanvas(canvas);

  m_player = player;
  m_total_time = 0;
  m_rect = rect;

  settings.qSettings()->beginGroup("Clock");

  QColor framecol(0x60,0x60,0x90);
  QColor backgroundColor;
  (settings["background"] |= QColor(0xa0,0xf0,0xd0,200)) >> backgroundColor;
  m_background = new Canvas::Rectangle(backgroundColor, QSize(m_rect.size()), this);
  m_frame[0] = new Canvas::Rectangle(framecol, QSize(m_rect.width()-2,1), this);
  m_frame[0]->moveTo(1,0);
  m_frame[1] = new Canvas::Rectangle(framecol, QSize(m_rect.width()-2,1), this);
  m_frame[1]->moveTo(0,m_rect.height()-1);
  m_frame[2] = new Canvas::Rectangle(framecol, QSize(1,m_rect.height()), this);
  m_frame[2]->moveTo(0,0);
  m_frame[3] = new Canvas::Rectangle(framecol, QSize(1,m_rect.height()), this);
  m_frame[3]->moveTo(m_rect.width()-1,0);

  int tempFontSize;

  {
    QFont captionFont("Bitstream Vera Sans");
    (settings["captionFontSize"] |=
      static_cast<int>(captionFont.pointSize() * 1.4)) >> tempFontSize;
    captionFont.setPointSize(tempFontSize);
    m_caption = new Canvas::Text(caption, Qt::black, captionFont,
        Canvas::Text::HStart, Canvas::Text::VTop, this);
    m_caption->show();
  }

  {
    QFont timeFont("Bitstream Vera Sans");
    (settings["timeFontSize"] |= timeFont.pointSize() * 2) >> tempFontSize;
    timeFont.setPointSize(tempFontSize);
    timeFont.setWeight(QFont::Bold);
    m_time_label = new Canvas::Text("", Qt::black, timeFont,
      Canvas::Text::HStart, Canvas::Text::VCenter, this);
    m_time_label->show();
  }

  {
    QFont decsFont("Bitstream Vera Sans");
    (settings["decsFontSize"] |=
      static_cast<int>(decsFont.pointSize() * 0.8)) >> tempFontSize;
    decsFont.setPointSize(tempFontSize);
    m_decs = new Canvas::Text("", Qt::black, decsFont,
      Canvas::Text::HStart, Canvas::Text::VBottom, this);
  }

  {
    QFont playerFont("Bitstream Vera Sans");
    (settings["playerFontSize"] |= playerFont.pointSize()) >> tempFontSize;
    playerFont.setPointSize(tempFontSize);
    m_player_name = new Canvas::Text(playerString(player), Qt::black, playerFont,
      Canvas::Text::HStart, Canvas::Text::VBottom, this);
    m_player_name->show();
  }
  settings.qSettings()->endGroup();

  computeTime();
  update();
  show();
}

void Clock::Info::reload() {
  settings.qSettings()->beginGroup("Clock");

  QFont tempFont;
  QColor backgroundColor;

  settings["background"] >> backgroundColor;
  m_background->setColor(backgroundColor);

  tempFont = m_caption->font();
  tempFont.setPointSize(settings["captionFontSize"].value<int>());
  m_caption->setFont(tempFont);

  tempFont = m_time_label->font();
  tempFont.setPointSize(settings["timeFontSize"].value<int>());
  m_time_label->setFont(tempFont);

  tempFont = m_decs->font();
  tempFont.setPointSize(settings["decsFontSize"].value<int>());
  m_decs->setFont(tempFont);

  tempFont = m_player_name->font();
  tempFont.setPointSize(settings["playerFontSize"].value<int>());
  m_player_name->setFont(tempFont);

  settings.qSettings()->endGroup();
}

QString Clock::Info::playerString(const Player& player) const {
  QString rating = player.rating != -1 ? QString(" (%1)").arg(player.rating) : "";
  return QString("%1").arg(player.name) + rating;
}

void Clock::Info::setPlayer(const Player& player) {
  m_player_name->setText(playerString(player));
}

void Clock::Info::setTime(int time) {
  m_total_time = time;
  tick();
}

void Clock::Info::resize(const QRect& rect) {
  m_rect = rect;
  update();
}

void Clock::Info::update() {
  m_background->setSize(m_rect.size());

  m_frame[0]->setSize(QSize(m_rect.width()-2,1));
  m_frame[0]->moveTo(1,0);
  m_frame[1]->setSize(QSize(m_rect.width()-2,1));
  m_frame[1]->moveTo(1,m_rect.height()-1);
  m_frame[2]->setSize(QSize(1,m_rect.height()));
  m_frame[2]->moveTo(0,0);
  m_frame[3]->setSize(QSize(1,m_rect.height()));
  m_frame[3]->moveTo(m_rect.width()-1,0);

  {
    /*QFont font = m_caption->font();
    setFontSize(20, m_rect.width() / 2, m_caption->text(), font);
    m_caption->setFont(font);*/
    m_caption->moveTo(QPoint(10, 10));
  }

  {
    QPoint pos(
      static_cast<int>(m_rect.width() * 0.5),
      static_cast<int>(m_rect.height() * 0.5));
    /*QFont font = m_time_label->font();
    int width = m_rect.width() - pos.x();
    setFontSize(22, width,
                m_time_label->text(), font);
    m_time_label->setFont(font);*/
    m_time_label->moveTo(pos);
  }

  m_player_name->moveTo(QPoint(
                  static_cast<int>(m_rect.width() * 0.05),
                  static_cast<int>(m_rect.height() * 0.8)));

  moveTo(m_rect.topLeft());
}

void Clock::Info::start() {
  m_running = true;
  m_time.start();
}

void Clock::Info::stop() {
  if (m_running) m_total_time -= m_time.elapsed();
  m_running = false;
}

void Clock::Info::computeTime() const {
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

    if (positive && decs != -1) {
      int dec = static_cast<int>(ceil(time / 100.0)) % 10;

      m_decs->moveTo(m_time_label->rect().bottomRight() + QPoint(2, 0));
      m_decs->setText(":" + QString::number(dec));
      m_decs->show();
    }
    else
      m_decs->hide();
  }

  m_time_label->setText(timeText);
  m_time_label->setColor(time <= 0 && m_running ? QColor(200,20,20) : Qt::black);
}

void Clock::Info::tick() {
  computeTime();
}

void Clock::Info::activate(bool value) {
  m_background->setVisible(value);
  for(int i=0;i<4;i++)
    m_frame[i]->setVisible(value);

  QColor textcolor = value ? Qt::black : Qt::darkGray;
  m_caption->setColor(textcolor);
  m_time_label->setColor(textcolor);
  m_decs->setColor(textcolor);
  m_player_name->setColor(textcolor);
}

QRect Clock::Info::eventRect() const {
  return m_background->rect().translated(pos());
}


Clock::Clock(Canvas::Abstract* parent)
: ClickableCanvas(parent)
, m_running(-1)
, m_active(-1) {
  QTime startup_time; startup_time.start();
  m_info[0].setup(Player(), QRect(0, 0, 0, 0), "White", this);
  m_info[1].setup(Player(), QRect(0, 0, 0, 0), "Black", this);
  connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void Clock::settingsChanged() {
  for(int i=0;i<2;i++)
    m_info[i].settingsChanged();
}

void Clock::resize(QSize size) {
  int baseWidth = (size.width() - 10) / 2;
  m_info[0].resize(QRect(0, 0, baseWidth, 70));
  m_info[1].resize(QRect(baseWidth + 10, 0, baseWidth, 70));
}

void Clock::reload() {
  m_info[0].reload();
  m_info[1].reload();
}

void Clock::setTime(int index, int value) {
  Q_ASSERT(index == 0 || index == 1);

  m_info[index].setTime(value);
}

void Clock::start(int index) {
  Q_ASSERT(index == 0 || index == 1);

  m_timer.start(10);
  m_running = index;
  m_info[index].start();
  m_info[1 - index].stop();
}

void Clock::stop() {
  m_info[0].stop();
  m_info[1].stop();
  m_timer.stop();
  m_running = -1;
}

void Clock::activate(int index) {
  m_active = index;
  m_info[0].activate(index == 0);
  m_info[1].activate(index == 1);
}

void Clock::tick() {
  if (m_running != -1) {
    Q_ASSERT(m_running == 0 || m_running == 1);
    m_info[m_running].tick();
  }
}

void Clock::setPlayers(const Player& white, const Player& black) {
  m_info[0].setPlayer(white);
  m_info[1].setPlayer(black);
}

void Clock::onMousePress(const QPoint& pos, int button) {
  if (button == Qt::LeftButton) {
    if (m_info[0].eventRect().contains(pos))
      emit labelClicked(0);
    else if (m_info[1].eventRect().contains(pos))
      emit labelClicked(1);
  }
}

#include "clock.moc"

