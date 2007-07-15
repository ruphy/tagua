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

#include <QPixmap>
#include <QTimer>
#include <QTime>
#include "kgamecanvas.h"
#include "player.h"
#include "pixmaploader.h"
#include "clickablecanvas.h"


class ConstrainedText;

class Clock : public QObject, public ClickableCanvas {
Q_OBJECT

  int m_color;
  QTimer m_timer;
  QTime m_time;
  int m_total_time;

  bool m_running;
  bool m_active;

  KGameCanvasPixmap* m_background;
  ConstrainedText* m_caption;
  ConstrainedText* m_time_label;
  ConstrainedText* m_player_name;
  ConstrainedText* m_decs;

  QPixmap m_active_pixmap;
  QPixmap m_inactive_pixmap;

  QColor m_active_text;
  QColor m_inactive_text;

  int m_height;


  /** the @a PixmapLoader used for controls */
  PixmapLoader m_controls_loader;


  void computeTime();
  static QString playerString(const Player& player);

public:
  Clock(int col, KGameCanvasAbstract* canvas);
  ~Clock();

  /** returns a reference to the loader used to load controls pixmaps */
  PixmapLoader* controlsLoader() { return &m_controls_loader; }

  bool running() { return m_running; }
  void start();
  void stop();
  bool active() { return m_active; }
  void activate(bool);

  void setPlayer(const Player& p);
  void setTime(int t);

  virtual void onMousePress(const QPoint& pos, int button);
  virtual void onMouseRelease(const QPoint& /*pos*/, int /*button*/) { }
  virtual void onMouseMove(const QPoint& /*pos*/, int /*button*/) { }

  void resize(int size);
  int height() { return m_height; }
  void settingsChanged() { }

private slots:
  void tick();

signals:
  void labelClicked(int);
};


#if 1-1

class Clock : public QObject, public ClickableCanvas {
Q_OBJECT

  /**
    * Structure containing information for a player.
    */
  class Info : public KGameCanvasGroup {
    Player m_player;
    QTime m_time;
    bool m_running;
    int m_total_time;
    KGameCanvasRectangle* m_background;
    KGameCanvasRectangle* m_frame[4];
    KGameCanvasText* m_caption;
    KGameCanvasText* m_time_label;
    KGameCanvasText* m_player_name;
    KGameCanvasText* m_decs;
    QRect m_rect;

    void computeTime() const;
    void update();
  public:
    Info()
    : m_player(QString(), 0)
    , m_running(false)
    , m_rect(0, 0, 0, 0) { }

    void setup(const Player& player, const QRect& p,
                const QString& caption, KGameCanvasAbstract* canvas);
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
  Clock(KGameCanvasAbstract* parent);

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

#endif

#endif // CLOCK_H
