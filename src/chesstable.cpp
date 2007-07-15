/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QLayout>
#include <QSplitter>
#include <QMouseEvent>
#include "chesstable.h"
#include "game.h"
#include "gameinfo.h"
#include "connection.h"
#include "piecepool.h"
#include "clock.h"
#include "global.h"
#include "movelist_table.h"
#include "infodisplay.h"

using namespace boost;

ChessTable::ChessTable(QWidget* parent)
: KGameCanvasWidget(parent)
, m_wallpaper(NULL)
, m_current(NULL)
, m_mousegrab(NULL)
, m_need_reload(false) {

  setMouseTracking(true);

  // create m_board
  m_board = new Board(this);
  m_board->show();

  // create move list
  m_movelist = new MoveList::Table;

  // create clocks
  for(int i=0;i<2;i++) {
    m_clocks[i] = new Clock(i, this);
    m_clocks[i]->show();
  }
  m_clocks[0]->activate(0);

  // create info display
  m_info = new InfoDisplay(this);
//  m_info->show();

  // create pools
  for(int i=0;i<2;i++) {
    m_pools[i] = new PiecePool(i, m_board, this);
    m_pools[i]->show();
  }

  m_board->raise();
  settingsChanged();
}

ChessTable::~ChessTable() {
  for(int i=0;i<2;i++)
  delete m_clocks[i];
  delete m_movelist;
  delete m_board;
  for(int i=0;i<2;i++)
    delete m_pools[i];
  delete m_info;
}

void ChessTable::settingsChanged() {
  m_board->settingsChanged();
  for(int i=0;i<2;i++)
    m_clocks[i]->settingsChanged();
  m_info->settingsChanged();
  for(int i=0;i<2;i++)
    m_pools[i]->settingsChanged();

  if(m_wallpaper)
    delete m_wallpaper;

  QPixmap bg = m_board->controlsLoader()->getStaticValue<QPixmap>("wallpaper");
  if(!bg.isNull()) {
    m_wallpaper = new KGameCanvasTiledPixmap(bg, QSize(), QPoint(), false, this);
    m_wallpaper->lower();
    m_wallpaper->show();
  }
  else
    m_wallpaper = 0;

  /* redo the layout, forcing reload */
  if(isVisible())
    layout(true);
  else
    m_need_reload = true;
}

ClickableCanvas* ChessTable::eventItemAt(QPoint pos) {
  if (m_board->boardRect().contains(pos))
    return m_board;

  for (int i=0; i<2; i++)
  if (m_pools[i]->boardRect().contains(pos))
    return m_pools[i];

  for (int i=0; i<2; i++)
  if (m_clocks[i]->rect().contains(pos))
    return m_clocks[i];

  return NULL;
}

void ChessTable::setEntity(const boost::shared_ptr<UserEntity>& entity) {
  m_board->setEntity(entity);
}

void ChessTable::layout(bool force_reload) {
  force_reload |= m_need_reload;
  m_need_reload = false;

  if (m_wallpaper) {
    m_wallpaper->setSize(size());
    QSize delta = (m_wallpaper->pixmap().size()-size())/2;
    m_wallpaper->setOrigin(QPoint(delta.width(), delta.height()));
  }

  ::LuaApi::LuaValueMap params;
  params["width"] = width();
  params["height"] = height();
  params["grid_size"] = QPointF(m_board->gridSize());

  ::LuaApi::LuaValueMap lvals = m_board->controlsLoader()->getStaticValue< ::LuaApi::LuaValueMap>("layout", &params);

  for(::LuaApi::LuaValueMap::iterator it = lvals.begin(); it != lvals.end(); ++it)
  if(double* val = boost::get<double>(&it.value()))
    std::cout << "lvals[" << it.key() << "] = " << *val << std::endl;
  else if(QPointF* val = boost::get<QPointF>(&it.value()))
    std::cout << "lvals[" << it.key() << "] = Point(" << val->x() << "," << val->y() << std::endl;
  else if(QRectF* val = boost::get<QRectF>(&it.value()))
    std::cout << "lvals[" << it.key() << "] = Rect(" << val->x() << "," << val->y()
                                            << "," << val->width() << "," << val->height() << std::endl;

#define GET_INT(name)                                    \
  int name = 0;                                          \
  if(double* val = boost::get<double>(&lvals[#name]))    \
    name = (int)*val;                                    \
  else                                                   \
    ERROR("Hey Jack, please set "<<#name<<" to a number in the layout!");

#define GET_POINT(name)                                  \
  QPoint name;                                           \
  if(QPointF* val = boost::get<QPointF>(&lvals[#name]))  \
    name = val->toPoint();                               \
  else                                                   \
    ERROR("Hey Jack, please set "<<#name<<" to a point in the layout!");

  GET_POINT(board_position);
  GET_INT(square_size);
  GET_INT(border_size);
  GET_POINT(clock0_position);
  GET_POINT(clock1_position);
  GET_INT(clock_size);
  GET_POINT(pool0_position);
  GET_POINT(pool1_position);
  GET_INT(pool_piece_size);

  m_board->moveTo(board_position.x(), board_position.y());
  m_board->onResize( square_size, border_size, force_reload);

  int x = !m_board->flipped();

  m_clocks[x]->resize(clock_size);
  m_clocks[x]->moveTo(clock0_position.x(), clock0_position.y());

  m_clocks[!x]->resize(clock_size);
  m_clocks[!x]->moveTo(clock1_position.x(), clock1_position.y());

  m_pools[x]->onResize(pool_piece_size);
  m_pools[x]->moveTo(pool0_position.x(), pool0_position.y());

  m_pools[!x]->onResize(pool_piece_size);
  m_pools[!x]->moveTo(pool1_position.x(), pool1_position.y());

//   Point gs = m_board->gridSize();
//   int sq_size = std::max(0, std::min(int(width()/(gs.x+2.6+(2.0/3.0)*3)),
//                         int(gs.y == 0 ? 100000 : height()/(gs.y+4.0/3.0))) );
//   int b = sq_size*2/3;
//   m_board->moveTo(b,b);
//   m_board->onResize( sq_size, force_reload);
//
//   int x = !m_board->flipped();
//
//   m_clocks[x]->resize(sq_size);
//   m_clocks[!x]->resize(sq_size);
//   m_clocks[x]->moveTo(sq_size*gs.x+2*b+b/2, b/2);
//   m_clocks[!x]->moveTo(sq_size*gs.x+2*b+b/2, sq_size*gs.y+b+b/2-m_clocks[1]->height());
//
//   m_pools[x]->m_flipped = false;
//   m_pools[x]->setGridWidth(3);
//   m_pools[x]->moveTo(sq_size*gs.x+2*b+b/2, b+m_clocks[0]->height());
//   m_pools[x]->onResize(static_cast<int>(sq_size*2.2/3), force_reload);
//
//   m_pools[!x]->m_flipped = true;
//   m_pools[!x]->setGridWidth(3);
//   m_pools[!x]->moveTo(sq_size*gs.x+2*b+b/2, sq_size*gs.y+b-m_clocks[1]->height());
//   m_pools[!x]->onResize(static_cast<int>(sq_size*2.2/3), force_reload);
//
//   m_info->moveTo(sq_size*gs.x+4*b, 80+sq_size*gs.y/2+b+b/2);
}

void ChessTable::resizeEvent(QResizeEvent* /*e*/) {
  layout();
}

void ChessTable::mouseReleaseEvent(QMouseEvent* e) {

  if(m_mousegrab) {
    m_mousegrab->onMouseRelease(e->pos() - m_mousegrab->pos(), e->button() );
    if(!e->buttons()) {
      m_mousegrab = NULL;

      ClickableCanvas* cb = eventItemAt(e->pos());
      if(cb != m_current) {
        if(m_current)
          m_current->onMouseLeave();
        if(cb) {
          cb->onMouseEnter();
          cb->onMouseMove(e->pos() - cb->pos(), 0);
        }
        m_current = cb;
      }
    }
    return;
  }
}

void ChessTable::mousePressEvent(QMouseEvent* e) {
  if(m_mousegrab) {
    m_mousegrab->onMousePress(e->pos() - m_mousegrab->pos(), e->button() );
    return;
  }

  ClickableCanvas* cb = eventItemAt(e->pos());
  if(cb != m_current) {
    if(m_current)
      m_current->onMouseLeave();
    if(cb)
      cb->onMouseEnter();
    m_current = cb;
  }
  if(cb) {
    cb->onMousePress(e->pos() - cb->pos(), e->button() );
    m_mousegrab = cb;
  }
}

void ChessTable::mouseMoveEvent(QMouseEvent* e) {
  if(m_mousegrab) {
    m_mousegrab->onMouseMove(e->pos() - m_mousegrab->pos(), e->button() );
    return;
  }

  ClickableCanvas* cb = eventItemAt(e->pos());
  if(cb != m_current) {
    if(m_current)
      m_current->onMouseLeave();
    if(cb)
      cb->onMouseEnter();
    m_current = cb;
  }
  if(cb)
    cb->onMouseMove(e->pos() - cb->pos(), e->button() );
}

void ChessTable::enterEvent(QEvent*) { }

void ChessTable::leaveEvent(QEvent*) {
  if(m_current)
    m_current->onMouseLeave();
  m_current = NULL;
}

void ChessTable::flip() {
  m_board->flip();

  int delta = qAbs(m_pools[0]->pos().y() - m_pools[1]->pos().y());
  for(int i=0;i<2;i++)
    m_pools[i]->flipAndMoveBy( QPoint(0, delta) );
}

void ChessTable::flip(bool flipped) {
  if(m_board->flipped() != flipped)
    flip();
}

void ChessTable::changeClock(int color) {
  std::cout << "change " << color << std::endl;
  if(m_clocks[0]->running() || m_clocks[1]->running())
  for(int i=0;i<2;i++) {
    if ( (i == color) != m_clocks[i]->running() )
    if( i==color )
      m_clocks[i]->start();
    else
      m_clocks[i]->stop();
  }
}

void ChessTable::updateTurn(int color) {
  std::cout << "updatet" << std::endl;
  for(int i=0;i<2;i++)
    m_clocks[i]->activate(color == i);
}

void ChessTable::stopClocks() {
  for(int i=0;i<2;i++)
    m_clocks[i]->stop();
}

void ChessTable::updateTime(int white, int black) {
  std::cout << "update" << std::endl;
  m_clocks[0]->setTime(white);
  m_clocks[1]->setTime(black);
}

void ChessTable::resetClock() {
  std::cout << "reset" << std::endl;
  stopClocks();
  updateTime(0, 0);
  for(int i=0;i<2;i++)
    m_clocks[i]->setPlayer(Player());
}

void ChessTable::setPlayers(const Player& white, const Player& black) {
  std::cout << "set players " << std::endl;
  m_clocks[0]->setPlayer(white);
  m_clocks[1]->setPlayer(black);
}

void ChessTable::run() {
  std::cout << "run" << std::endl;
  for(int i=0;i<2;i++)
  if(m_clocks[i]->active() && !m_clocks[i]->running())
    m_clocks[i]->start();
}

void ChessTable::displayMessage(const QString& msg) {
  std::cout << msg << std::endl;
  emit message(msg);
}


