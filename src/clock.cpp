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

#if 1-1
#include <math.h>
#include <iostream>
#include <QResizeEvent>
#include "clock.h"
#include "mastersettings.h"

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

void Clock::Info::setup(const Player& player, const QRect& rect, const QString& caption, KGameCanvasAbstract* canvas) {
  putInCanvas(canvas);

  m_player = player;
  m_total_time = 0;
  m_rect = rect;

  Settings s_clock = settings().group("clock");

  QColor framecol(0x60,0x60,0x90);
  QColor backgroundColor;
  (s_clock["background"] |= QColor(0xa0,0xf0,0xd0,200)) >> backgroundColor;
  m_background = new KGameCanvasRectangle(backgroundColor, QSize(m_rect.size()), this);
  m_frame[0] = new KGameCanvasRectangle(framecol, QSize(m_rect.width()-2,1), this);
  m_frame[0]->moveTo(1,0);
  m_frame[1] = new KGameCanvasRectangle(framecol, QSize(m_rect.width()-2,1), this);
  m_frame[1]->moveTo(0,m_rect.height()-1);
  m_frame[2] = new KGameCanvasRectangle(framecol, QSize(1,m_rect.height()), this);
  m_frame[2]->moveTo(0,0);
  m_frame[3] = new KGameCanvasRectangle(framecol, QSize(1,m_rect.height()), this);
  m_frame[3]->moveTo(m_rect.width()-1,0);

  int tempFontSize;

  {
    QFont captionFont("Bitstream Vera Sans");
    (s_clock["captionFontSize"] |=
      static_cast<int>(captionFont.pointSize() * 1.4)) >> tempFontSize;
    captionFont.setPointSize(tempFontSize);
    m_caption = new KGameCanvasText(caption, Qt::black, captionFont,
        KGameCanvasText::HStart, KGameCanvasText::VTop, this);
    m_caption->show();
  }

  {
    QFont timeFont("Bitstream Vera Sans");
    (s_clock["timeFontSize"] |= timeFont.pointSize() * 2) >> tempFontSize;
    timeFont.setPointSize(tempFontSize);
    timeFont.setWeight(QFont::Bold);
    m_time_label = new KGameCanvasText("", Qt::black, timeFont,
      KGameCanvasText::HStart, KGameCanvasText::VCenter, this);
    m_time_label->show();
  }

  {
    QFont decsFont("Bitstream Vera Sans");
    (s_clock["decsFontSize"] |=
      static_cast<int>(decsFont.pointSize() * 0.8)) >> tempFontSize;
    decsFont.setPointSize(tempFontSize);
    m_decs = new KGameCanvasText("", Qt::black, decsFont,
      KGameCanvasText::HStart, KGameCanvasText::VBottom, this);
  }

  {
    QFont playerFont("Bitstream Vera Sans");
    (s_clock["playerFontSize"] |= playerFont.pointSize()) >> tempFontSize;
    playerFont.setPointSize(tempFontSize);
    m_player_name = new KGameCanvasText(playerString(player), Qt::black, playerFont,
      KGameCanvasText::HStart, KGameCanvasText::VBottom, this);
    m_player_name->show();
  }

  computeTime();
  update();
  show();
}

void Clock::Info::reload() {
  Settings s_clock = settings().group("clock");

  QFont tempFont;
  QColor backgroundColor;

  s_clock["background"] >> backgroundColor;
  m_background->setColor(backgroundColor);

  tempFont = m_caption->font();
  tempFont.setPointSize(s_clock["captionFontSize"].value<int>());
  m_caption->setFont(tempFont);

  tempFont = m_time_label->font();
  tempFont.setPointSize(s_clock["timeFontSize"].value<int>());
  m_time_label->setFont(tempFont);

  tempFont = m_decs->font();
  tempFont.setPointSize(s_clock["decsFontSize"].value<int>());
  m_decs->setFont(tempFont);

  tempFont = m_player_name->font();
  tempFont.setPointSize(s_clock["playerFontSize"].value<int>());
  m_player_name->setFont(tempFont);
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


Clock::Clock(KGameCanvasAbstract* parent)
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
      labelClicked(0);
    else if (m_info[1].eventRect().contains(pos))
      labelClicked(1);
  }
}

#endif
