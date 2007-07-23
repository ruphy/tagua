/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CHESSTABLE_H
#define CHESSTABLE_H

#include <qwidget.h>

#include "kgamecanvas.h"
#include "positioninfo.h"
#include "board.h"
#include "common.h"
#include "animationfactory.h"

class PiecePool;
class Player;
class Clock;
class InfoDisplay;
namespace MoveList {
  class Table;
}
namespace Canvas {
  class TiledPixmap;
}

class ChessTable : public KGameCanvasWidget {
  Q_OBJECT
  KGameCanvasTiledPixmap* m_wallpaper;

  Clock*           m_clocks[2];
  Board*           m_board;
  PiecePool*       m_pools[2];
  MoveList::Table* m_movelist;
  InfoDisplay*     m_info;

  ClickableCanvas*      m_current;
  ClickableCanvas*      m_mousegrab;
  bool             m_need_reload;

  ClickableCanvas*    eventItemAt(QPoint pos);

  void layout(bool force_reload = false);

  AnimationSettings m_anim_settings;
public:
  ChessTable(QWidget* parent = 0);
  ~ChessTable();

  inline Board*           board() const { return m_board; }
  inline PiecePool*       pool(int index) const { return m_pools[index]; }
  inline MoveList::Table* moveListTable() const { return m_movelist; }
  inline Clock*           clock(int index) const { return m_clocks[index]; }
  inline InfoDisplay*     info() const { return m_info; }

  void flip(bool);
  void flip();
  void run();
  void stopClocks();

  void setEntity(const boost::shared_ptr<UserEntity>& entity);

  void displayMessage(const QString&);

  virtual void resizeEvent(QResizeEvent*);
  virtual void mouseReleaseEvent(QMouseEvent*);
  virtual void mousePressEvent(QMouseEvent*);
  virtual void mouseMoveEvent(QMouseEvent*);
  virtual void enterEvent (QEvent * event);
  virtual void leaveEvent (QEvent * event);

  void settingsChanged(); //called by GraphicalInfo, for better theme change
  const AnimationSettings& animationSettings() const;
public Q_SLOTS:
  void updateTurn(int color);
  void changeClock(int color);
  void updateTime(int white, int black);
  void setPlayers(const Player&, const Player&);
  void resetClock();

Q_SIGNALS:
  void message(const QString&);
  void error(ErrorCode);
};

#endif // CHESSTABLE_H
