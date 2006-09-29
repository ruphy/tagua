/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CLOCK_H
#define CLOCK_H

#include <boost/array.hpp>
#include <QTimer>
#include <QTime>
#include "canvas/canvas.h"
#include "player.h"
#include "clickablecanvas.h"

class Clock : public QObject, public ClickableCanvas {
Q_OBJECT
  /**
    * Structure containing information for a player.
    */
  class Info : public Canvas::Group {
    Player m_player;
    QTime m_time;
    bool m_running;
    int m_total_time;
    Canvas::Rectangle* m_background;
    Canvas::Rectangle* m_frame[4];
    Canvas::Text* m_caption;
    Canvas::Text* m_time_label;
    Canvas::Text* m_player_name;
    Canvas::Text* m_decs;
    QRect m_rect;

    void computeTime() const;
    void update();
  public:
    Info()
    : m_player(QString(), 0)
    , m_running(false)
    , m_rect(0, 0, 0, 0) { }

    void setup(const Player& player, const QRect& p,
                const QString& caption, Canvas::Abstract* canvas);
    void setTime(int);
    void start();
    void stop();
    void activate(bool);
    void tick();

    QString playerString(const Player& player) const;
    void setPlayer(const Player& player);

    void resize(const QRect& rect);
    void reload();
    QRect eventRect() const;

    /** changed settings handler */
    virtual void settingsChanged();
  };

  Info m_info[2];
  QTimer m_timer;
  int m_running;
  int m_active;
public:
  Clock(Canvas::Abstract* parent);

  void setTime(int index, int value);
  void start(int index);
  void stop();
  void activate(int index);

  void setPlayers(const Player& white, const Player& black);

  bool running() const { return m_running != -1; }
  void resize(QSize size);
  void reload();

  virtual void onMousePress(const QPoint& pos, int button);
  virtual void onMouseRelease(const QPoint& /*pos*/, int /*button*/) { }
  virtual void onMouseMove(const QPoint& /*pos*/, int /*button*/) { }

  /** changed settings handler */
  virtual void settingsChanged();
private slots:
  void tick();
signals:
  void labelClicked(int);
};

#endif // CLOCK_H
