/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include "board.h"
#include "animation.h"
#include "piecepool.h"

PiecePool::PiecePool(Board* b, Canvas::Abstract* parent)
: PieceGroup(parent)
, m_sprites(0,0)
, m_board(b)
, m_fill(0) {
  setGridWidth(1);
}

PiecePool::~PiecePool() {
}

void PiecePool::settingsChanged() {
  PieceGroup::settingsChanged();
}

Point PiecePool::flipPoint(const Point& p) const {
  Point retv = p;
  if((retv.y>=0 ? retv.y : -1-retv.y) % 2 == 1)
    retv.x = gridSize().x-1-retv.x;
  if(flipped())
    retv.y = -1-retv.y;
  return retv;
}

void PiecePool::setGridWidth(int w) {
  m_sprites.changeSize(w, (m_fill+w-1)/w);
}

int PiecePool::fill() {
  return m_fill;
}

void PiecePool::setFill(int f) {
  int w = gridSize().x;
  if(!w) {
    m_sprites.changeSize(0,0);
    m_fill = 0;
  }
  else {
    m_sprites.changeSize(w, (f+w-1)/w);
    m_fill = f;
  }
}

void PiecePool::clear() {
  setFill(0);
}

void PiecePool::insertSprite(int index, const NamedSprite& nsprite) {
  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index > fill() ) {
    std::cout << "invalid index " << index << " in PiecePool::insertPiece" << std::endl;
    return;
  }

  setFill(m_fill+1);
  Point i = m_sprites.nTh(index);

  NamedSprite replacep = m_sprites[i];
  m_sprites[i] = nsprite;
  m_sprites[i].sprite()->show();
  m_sprites[i].sprite()->moveTo(converter()->toReal(i));
  fadeIn(i);

  int speed = 1;
  for( i=m_sprites.next(i); i<=m_sprites.last(); i=m_sprites.next(i)) {
    if(!replacep)
      break;
    NamedSprite tmp = replacep;
    replacep = m_sprites[i];
    m_sprites[i] = tmp;
    animatePiece(m_sprites[i].sprite(), i, (1.0+1.0/speed)*0.4);
    speed++;
  }
}

NamedSprite PiecePool::getSprite(int index) {
  if(m_dragged && index == m_dragged_index)
    return m_dragged;

  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= fill() ) {
    ERROR("invalid index " << index);
    return NamedSprite();
  }

  Point i = m_sprites.nTh(index);
  return m_sprites[i];
}

NamedSprite PiecePool::takeSprite(int index) {
  if(m_dragged && index == m_dragged_index) {
    NamedSprite retv = m_dragged;
    m_dragged = NamedSprite();
    m_dragged_index = -1;

    return retv;
  }

  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= fill() ) {
    ERROR("invalid index " << index);
    return NamedSprite();
  }

  Point i = m_sprites.nTh(index);
  NamedSprite piece = takeNamedSprite(i);

  return piece;
}

NamedSprite PiecePool::takeNamedSprite(const Point& _i) {
  Point i = _i;
  if(i < m_sprites.first() || m_sprites.last() < i ) {
    ERROR("invalid index " << i);
    return NamedSprite();
  }

  NamedSprite piece = m_sprites[i];
  if(!piece.sprite())
    return NamedSprite();

  Point previ = i;
  int speed = 1;
  i = m_sprites.next(i);
  for(; i<=m_sprites.last(); i=m_sprites.next(i)) {
    m_sprites[previ] = m_sprites[i];

    if(m_sprites[previ]) {
      animatePiece(m_sprites[previ].sprite(), previ, (1.0+1.0/speed)*0.4);
      speed++;
    }

    previ = i;
  }
  m_sprites[previ] = NamedSprite();
  setFill(m_fill-1);

  return piece;
}

Canvas::Abstract* PiecePool::piecesGroup() {
  return this;
}

void PiecePool::clearDrag(bool fadeOff) {
  if(!m_dragged)
    return;

  m_dragged.sprite()->setPixmap( m_loader( m_dragged.name() ) );
  m_dragged.sprite()->putInCanvas(piecesGroup());

  if (fadeOff) {
    SpritePtr phantom = SpritePtr(m_dragged.sprite()->duplicate());
    if(m_anim_fade)
      enqueue( boost::shared_ptr<Animation>(new FadeAnimation(phantom, phantom->pos(), 255, 0)) );
    else
      enqueue( boost::shared_ptr<Animation>(new CaptureAnimation(phantom)) );
  }

  addPiece(m_dragged_index, m_dragged);

  m_dragged = NamedSprite();
  m_dragged_index = -1;
}

void PiecePool::flipAndMoveBy(QPoint p) {
  QPoint deltapos = m_flipped ? -p : p;
  moveTo(pos() + deltapos);
  m_flipped = !m_flipped;

  for(Point i=m_sprites.first(); i<=m_sprites.last(); i=m_sprites.next(i))
  if(m_sprites[i]) {
    if(m_anim_movement) {
      enqueue(boost::shared_ptr<Animation>(new InstantAnimation(m_sprites[i].sprite(),
                                               m_sprites[i].sprite()->pos() - deltapos)));
      enqueue(boost::shared_ptr<Animation>(new MovementAnimation(m_sprites[i].sprite(),
                                               converter()->toReal(i), 1.0)));
    }
    else
      enqueue(boost::shared_ptr<Animation>(new InstantAnimation(m_sprites[i].sprite(),
                                               converter()->toReal(i))));
  }
}

void PiecePool::updateSprites() {
  // adjust piece positions
  for (Point i = m_sprites.first(); i <= m_sprites.last(); i = m_sprites.next(i)) {
    boost::shared_ptr<Sprite> p = m_sprites[i].sprite();

    if (p) {
      // drawing sprite
      p->setPixmap( m_loader( m_sprites[i].name() ) );
      adjustSprite(i);
    }
  }
}

void PiecePool::onResize(int new_size, bool force_reload) {
  if(m_square_size == new_size && !force_reload)
    return;

  PieceGroup::onResize(new_size);

  // update the sprites
  updateSprites();
}

void PiecePool::onMouseRelease(const QPoint& pos, int button) {
  if (button != Qt::LeftButton || !m_dragged)
    return;

  /* did the board take this sprite? */
  m_board->dropOn( 0/*BROKEN*/, m_dragged_index, pos + this->pos() - m_board->pos() );

  bool fadeOff = true;
  if(!m_board->m_drop_sprite && m_dragged) {
    m_dragged = NamedSprite( m_dragged.name(), SpritePtr(m_dragged.sprite()->duplicate()) );
    fadeOff = false;
  }
  m_board->m_drop_sprite = NamedSprite();
  clearDrag(fadeOff);
}

void PiecePool::onMousePress(const QPoint& pos, int button) {
  if (button != Qt::LeftButton)
    return;

  if(m_dragged) {
    std::cout << "Eh? We are already dragging?" << std::endl;
    m_board->m_drop_sprite = NamedSprite();
    clearDrag(); //never remove implicitly a piece from the pool
  }

  Point p = converter()->toLogical(pos);
  NamedSprite got = takeNamedSprite(p);
  if(!got)
    return;

  got.sprite()->hide();

  /* recreate the sprite, as "got" may be being animated */
  QPixmap px = m_board->m_loader( got.name() );
  QPoint at = pos + this->pos() - m_board->pos() - QPoint(px.width(), px.height())/2;
  m_dragged = NamedSprite(  got.name(),  SpritePtr(new Sprite(px, m_board->piecesGroup(), at)) );
  m_dragged.sprite()->raise();
  m_dragged.sprite()->show();
  m_board->m_drop_sprite = m_dragged;
}

void PiecePool::onMouseMove(const QPoint& pos, int /*button*/) {
  if(m_dragged) {
    m_dragged.sprite()->moveTo(pos + this->pos() - m_board->pos()
                - QPoint(m_dragged.sprite()->pixmap().width(),
                         m_dragged.sprite()->pixmap().height() ) / 2 );
    //BROKEN m_board->draggingOn( m_dragged.piece(), pos + this->pos() - m_board->pos() );
  }
}
