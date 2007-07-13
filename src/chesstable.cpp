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

  // create clock
  m_clock = new Clock(this);
  m_clock->show();
  m_clock->activate(0);


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
  delete m_clock;
  delete m_movelist;
  delete m_board;
  for(int i=0;i<2;i++)
    delete m_pools[i];
  delete m_info;
}

void ChessTable::settingsChanged() {
  m_board->settingsChanged();
  m_clock->settingsChanged();
  m_info->settingsChanged();
  for(int i=0;i<2;i++)
    m_pools[i]->settingsChanged();

  if(m_wallpaper)
    delete m_wallpaper;
  bool was0 = m_board->squareSize() == 0;
  if(was0) m_board->tagsLoader()->setSize(1);
  QPixmap bg = m_board->tagsLoader()->operator()("wallpaper");
  if(was0) m_board->tagsLoader()->setSize(0);
  if(!bg.isNull()) {
    std::cout << "Size is " << bg.size().width() << " "<< bg.size().height()  << std::endl;
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

  if (m_clock->rect().contains(pos))
    return m_clock;

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

  Point gs = m_board->gridSize();
  int sq_size = std::max(0, std::min(int(width()/(gs.x+2.2+4.0/3.0)),
                        int(gs.y == 0 ? 100000 : (height()-80)/(gs.y+6.0/3.0))) );
  int b = sq_size*2/3;
  m_board->moveTo(b,80+2*b);
  m_board->onResize( sq_size, force_reload);

  m_clock->moveTo(0, b/2);
  m_clock->resize(QSize(m_board->rect().width(), 80) /*, force_reload*/);

  int x = !!m_board->flipped();
  m_pools[x]->m_flipped = false;
  m_pools[x]->setGridWidth(3);
  m_pools[x]->moveTo(sq_size*gs.x+2*b+b/2, 80+b+b/2);
  m_pools[x]->onResize(static_cast<int>(sq_size*2.2/3), force_reload);

  x = !x;
  m_pools[x]->m_flipped = true;
  m_pools[x]->setGridWidth(3);
  m_pools[x]->moveTo(sq_size*gs.x+2*b+b/2, 80+sq_size*gs.y+b*2+b/2);
  m_pools[x]->onResize(static_cast<int>(sq_size*2.2/3), force_reload);

  m_info->moveTo(sq_size*gs.x+4*b, 80+sq_size*gs.y/2+b+b/2);
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

  for(int i=0;i<2;i++)
    m_pools[i]->flipAndMoveBy( QPoint(0, m_board->squareSize()*8+m_board->squareSize()*2/3) );
}

void ChessTable::flip(bool flipped) {
  if(m_board->flipped() != flipped)
    flip();
}

void ChessTable::changeClock(int color) {
  if (m_clock->running()) {
    m_clock->start(color);
  }
}

void ChessTable::updateTurn(int color) {
  m_clock->activate(color);
}

void ChessTable::stopClocks() {
  m_clock->stop();
}

void ChessTable::updateTime(int white, int black) {
  m_clock->setTime(0, white);
  m_clock->setTime(1, black);
}

void ChessTable::resetClock() {
  stopClocks();
  updateTime(0, 0);
  m_clock->setPlayers(Player(), Player());
}

void ChessTable::setPlayers(const Player& white, const Player& black) {
  m_clock->setPlayers(white, black);
}

void ChessTable::run() {
  if (!m_clock->running()) m_clock->start(0);
}

void ChessTable::displayMessage(const QString& msg) {
  std::cout << msg << std::endl;
  emit message(msg);
}


