/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include "piecepool.h"
#include "board.h"
#include <KDebug>

PiecePool::PiecePool(int num, Board* b, const AnimationSettings& animSettings, KGameCanvasAbstract* parent)
: ClickableCanvas(parent)
, m_pool_num(num)
, m_board(b)
, m_flipped(false)
, m_square_size(0)
, m_width(1)
, m_dragged_index(-1)
, m_anim_settings(animSettings) {
  m_main_animation = new MainAnimation( 1.0 );
  setGridWidth(1);
}


PiecePool::~PiecePool() {
  delete m_main_animation;
}


QPoint PiecePool::toReal(int i) const {
  int x = i % m_width;
  int y = i / m_width;
  
  if (y & 1)
    x = m_width - 1 - x;
    
  if (m_flipped)
    y = -1 - y;

  return QPoint(x, y) * m_square_size;
}


int PiecePool::toLogical(const QPoint& p) const {
  int x = p.x() / m_square_size;
  int y = p.y() / m_square_size;

  if (x<0 || x >= m_width)
    return -1;
    
  if (m_flipped)
    y = -y;

  if (y & 1)
    x = m_width - 1 - x;

  int retv = y * m_width + x;
  if (retv < 0 || static_cast<unsigned int>(retv) >= m_sprites.size())
    return -1;
    
  return retv;
}


void PiecePool::settingsChanged() {
  
}


void PiecePool::setGridWidth(int w) {
  m_width = w > 0 ? w : 1;
}

QRect PiecePool::boardRect() {
  Q_ASSERT(m_width > 0);
  int height = (m_sprites.size() + m_width - 1) / m_width;
  QSize sz(m_width, m_flipped ? -height : height);
  return QRect(pos(), sz * m_square_size);
}


int PiecePool::fill() {
  return m_sprites.size();
}


void PiecePool::clear() {
  m_sprites.clear();
}

void PiecePool::animate(const Animate::Pool::Scheme& scheme, Animate::AnimationType type) {
  m_main_animation->addAnimation(scheme.run(m_anim_settings, this, type));
}

void PiecePool::insertSprite(int index, const NamedSprite& nsprite) {
  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index > fill() ) {
    kError() << "invalid index" << index;
    return;
  }

  m_sprites.resize(m_sprites.size()+1);

  for(int i = m_sprites.size()-1; i > index; i--) {
//     double speed = (1.0 + 1.0 / (i - index + 1)) * 0.4;
    m_sprites[i] = m_sprites[i-1];
    animate(Animate::Pool::move(m_sprites[i], i));
  }

  m_sprites[index] = nsprite;
  animate(Animate::Pool::move(m_sprites[index], index), Animate::Instant);
  animate(Animate::Pool::appear(m_sprites[index]));
}


NamedSprite PiecePool::getSprite(int index) {
  if(m_dragged && index == m_dragged_index)
    return m_dragged;

  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= (int)m_sprites.size() ) {
    kError() << "invalid index" << index;
    return NamedSprite();
  }

  return m_sprites[index];
}


void PiecePool::removeSprite(int index) {
  if(m_dragged && index == m_dragged_index) {
    m_dragged = NamedSprite();
    m_dragged_index = -1;
    return;
  }

  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= (int)m_sprites.size() ) {
    kError() << "invalid index" << index;
    return;
  }

  takeSpriteAt(index);
}


NamedSprite PiecePool::takeSprite(int index) {
  if(m_dragged && index == m_dragged_index) {
    NamedSprite retv = m_dragged;
    m_dragged = NamedSprite();
    m_dragged_index = -1;

    return retv;
  }

  kError() << "Only the sprite being dropped can be taken from the pool.";
  return NamedSprite();
#if 0
  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= (int)m_sprites.size() ) {
    kError() << "invalid index" << index;
    return NamedSprite();
  }

  return takeSpriteAt(index);
#endif
}


NamedSprite PiecePool::takeSpriteAt(int index) {
  if(index < 0 || index >= (int)m_sprites.size() ) {
    kError() << "invalid index" << index;
    return NamedSprite();
  }

  NamedSprite retv = m_sprites[index];
  if (!retv)
    return NamedSprite();
  animate(Animate::Pool::disappear(retv));

  for(int i = index; i < (int)m_sprites.size()-1; i++) {
//     double speed = (1.0 + 1.0 / (i - index + 1)) * 0.4;
    m_sprites[i] = m_sprites[i+1];
    animate(Animate::Pool::move(m_sprites[i], i));
  }
  m_sprites.resize(m_sprites.size()-1);

  return retv;
}


void PiecePool::cancelDragging(bool fadeOff) {
  if(!m_dragged)
    return;

  if (fadeOff) {
    NamedSprite phantom = m_dragged.duplicate();    
    animate(Animate::Pool::disappear(phantom));
  }

  m_dragged.sprite()->putInCanvas(this);
  m_dragged.sprite()->setPixmap(loadSprite(m_dragged.name()));
  insertSprite(m_dragged_index, m_dragged);

  m_dragged = NamedSprite();
  m_dragged_index = -1;
}


void PiecePool::flipAndMoveBy(QPoint p) {
  QPoint deltapos = m_flipped ? -p : p;
  moveTo(pos() + deltapos);
  m_flipped = !m_flipped;

  for(int i=0;i<(int)m_sprites.size(); i++) {
    SpritePtr p = m_sprites[i].sprite();
    p->setPixmap(loadSprite(m_sprites[i].name()));
    animate(Animate::Pool::move(m_sprites[i], i), Animate::Instant);
  }
}


void PiecePool::onResize(int new_size, bool force_reload) {
  if (m_square_size == new_size && !force_reload)
    return;

  m_square_size = new_size;
  m_loader.setSize(m_square_size);

  // update the sprites
  for (int i=0;i<(int)m_sprites.size(); i++) {
    m_sprites[i].sprite()->setPixmap(loadSprite(m_sprites[i].name()));
    m_main_animation->addAnimation(AnimationPtr(new InstantAnimation(m_sprites[i].sprite(), toReal(i))));
  }
}


void PiecePool::onMouseRelease(const QPoint& pos, int button) {
  if (button != Qt::LeftButton || !m_dragged)
    return;

  m_board->m_dropped_pool = m_pool_num;
  m_board->m_dropped_index = m_dragged_index;

  /* did the board take this sprite? */
  m_board->dropOn( m_pool_num, m_dragged_index, pos + this->pos() - m_board->pos() );

  m_board->m_dropped_pool = -1;
  m_board->m_dropped_index = -1;

  cancelDragging(true);
}


void PiecePool::onMousePress(const QPoint& pos, int button) {
  if (button != Qt::LeftButton)
    return;

  if(m_dragged) {
    kError() << "Eh? We are already dragging?";
    cancelDragging(); //never remove implicitly a piece from the pool
  }

  int index = toLogical(pos);
  if(index == -1)
    return;
  NamedSprite got = takeSpriteAt(index);
  if(!got)
    return;

  got.sprite()->hide();

  /* recreate the sprite, as "got" may be being animated */
  QPixmap px = m_board->loadSprite(got.name());
  QPoint at = pos + this->pos() - m_board->pos() - QPoint(px.width(), px.height())/2;
  m_dragged = NamedSprite(  got.name(), SpritePtr(new Sprite(px, m_board->piecesGroup(),  at) ) );
  m_dragged.sprite()->raise();
  m_dragged.sprite()->show();
  m_dragged_index = index;
}


void PiecePool::onMouseMove(const QPoint& pos, int /*button*/) {
  if(m_dragged) {
    m_dragged.sprite()->moveTo(pos + this->pos() - m_board->pos()
                - QPoint(m_dragged.sprite()->pixmap().width(),
                         m_dragged.sprite()->pixmap().height() ) / 2 );
    m_board->draggingOn( m_pool_num, m_dragged_index, pos + this->pos() - m_board->pos() );
  }
}

QPixmap PiecePool::loadSprite(const QString& id) {
  // use board flipped state here, because the pool
  // flipping only refers to the displaying of pieces
  // and should not affect their orientation (which should
  // stay coherent with that of the pieces on the board).
  return m_loader.piecePixmap(id, m_board->flipped());
}

