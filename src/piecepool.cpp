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

/*****************************************************************************************/
PiecePool::PiecePool(Board* b, Canvas::Abstract* parent)
: PieceGroup(parent)
, m_board(b)
, m_flipped(false)
, m_square_size(0)
, m_width(1)
, m_fill(0)
, m_dragged_index(-1) {
  setGridWidth(1);
}

/*****************************************************************************************/
PiecePool::~PiecePool() {
}

/*****************************************************************************************/
void PiecePool::settingsChanged() {
  PieceGroup::settingsChanged();
}

/*****************************************************************************************/
Point PiecePool::flipPoint(const Point& p) const {
  Point retv = p;
  if((retv.y>=0 ? retv.y : -1-retv.y) % 2 == 1)
    retv.x = gridSize().x-1-retv.x;
  if(flipped())
    retv.y = -1-retv.y;
  return retv;
}

/*****************************************************************************************/
void PiecePool::setGridWidth(int w) {
  m_width = w;
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

/*****************************************************************************************/
void PiecePool::clear() {
  setFill(0);
}

/*****************************************************************************************/
void PiecePool::insertSprite(int index, const NamedSprite& nsprite) {
  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index > fill() ) {
    ERROR("invalid index " << index);
    return;
  }

  m_sprite.resize(m_sprites.size()+1);

  for(int i = m_sprites.size()-2; i >= index; i--) {
    double speed = (1.0+1.0/(i - index + 1))*0.4;
    m_sprites[i] = m_sprites[i-1];
    m_sprites[i]->moveTo(toReal(i)); //BROKEN animate to that point?
  }

  m_sprites[index] = nsprite;
  m_sprites[index].sprite()->show();
  m_sprites[index].sprite()->moveTo(toReal(index));
  //BROKEN fadeIn(index);
}

/*****************************************************************************************/
NamedSprite PiecePool::getSprite(int index) {
  if(m_dragged && index == m_dragged_index)
    return m_dragged;

  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= m_sprites.size() ) {
    ERROR("invalid index " << index);
    return NamedSprite();
  }

  return m_sprites[index];
}

/*****************************************************************************************/
NamedSprite PiecePool::takeSprite(int index) {
  if(m_dragged && index == m_dragged_index) {
    NamedSprite retv = m_dragged;
    m_dragged = NamedSprite();
    m_dragged_index = -1;

    return retv;
  }

  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index >= m_sprites.size() ) {
    ERROR("invalid index " << index);
    return NamedSprite();
  }

  return takeSpriteAt(index);
}

/*****************************************************************************************/
NamedSprite PiecePool::takeSpriteAt(int index) {
  if(index < 0 || index >= m_sprites.size() ) {
    ERROR("invalid index " << index);
    return NamedSprite();
  }

  NamedSprite retv = m_sprites[index];
  if(!retv)
    return NamedSprite();

  for(int i = index; i < m_sprites.size()-1; i++) {
    double speed = (1.0+1.0/(i - index + 1))*0.4;
    m_sprites[i] = m_sprites[i+1];
    m_sprites[i]->moveTo(toReal(i)); //BROKEN animate to that point?
  }
  m_sprites.resize(m_sprites.size()-1);

  return retv;
}

/*****************************************************************************************/
Canvas::Abstract* PiecePool::piecesGroup() {
  return this;
}

/*****************************************************************************************/
void PiecePool::cancelDragging(bool fadeOff) {
  if(!m_dragged)
    return;

  m_dragged.sprite()->setPixmap( m_loader( m_dragged.name() ) );
  m_dragged.sprite()->putInCanvas(piecesGroup());

  if (fadeOff) {
    SpritePtr phantom = SpritePtr(m_dragged.sprite()->duplicate());
    if(m_anim_fade)
      enqueue( AnimationPtr(new FadeAnimation(phantom, phantom->pos(), 255, 0)) );
    else
      enqueue( AnimationPtr(new CaptureAnimation(phantom)) );
  }

  insertSprite(m_dragged_index, m_dragged);

  m_dragged = NamedSprite();
  m_dragged_index = -1;
}

/*****************************************************************************************/
void PiecePool::flipAndMoveBy(QPoint p) {
  QPoint deltapos = m_flipped ? -p : p;
  moveTo(pos() + deltapos);
  m_flipped = !m_flipped;

  for(int i=0;i<m_sprites.size(); i++)
  if(m_anim_movement) {
    enqueue(AnimationPtr(new InstantAnimation(m_sprites[i].sprite(),
                                              m_sprites[i].sprite()->pos() - deltapos)));
    enqueue(AnimationPtr(new MovementAnimation(m_sprites[i].sprite(),
                                              toReal(i), 1.0)));
  }
  else
    enqueue(AnimationPtr(new InstantAnimation(m_sprites[i].sprite(),
                                              toReal(i))));
}

/*****************************************************************************************/
void PiecePool::onResize(int new_size, bool force_reload) {
  if(m_square_size == new_size && !force_reload)
    return;

  m_square_size = new_size;
  m_loader.setSize(m_square_size);

  // update the sprites
  for (int i=0;i<m_sprites.size(); i++) {
    SpritePtr s = m_sprites[i].sprite();

    if (p) { // should be always true
      // drawing sprite
      s->setPixmap( m_loader( m_sprites[i].name() ) );
      enqueue(AnimationPtr(new InstantAnimation(s, toReal(i))));
    }
  }
}

/*****************************************************************************************/
void PiecePool::onMouseRelease(const QPoint& pos, int button) {
  if (button != Qt::LeftButton || !m_dragged)
    return;

  /* did the board take this sprite? */
  m_board->dropOn( 0/*BROKEN*/, m_dragged_index, pos + this->pos() - m_board->pos() );

#if 0
  bool fadeOff = true;
  if(!m_board->m_drop_sprite && m_dragged) {

    //this happens if the animator used the piece being dropped but removed another piece from the pool
    //for instance it remove another piece of the same type.
    m_dragged = NamedSprite( m_dragged.name(), SpritePtr(m_dragged.sprite()->duplicate()) );
    fadeOff = false;
  }
  m_board->m_drop_sprite = NamedSprite();
  cancelDragging(fadeOff);
#endif

  cancelDragging(true);
}

/*****************************************************************************************/
void PiecePool::onMousePress(const QPoint& pos, int button) {
  if (button != Qt::LeftButton)
    return;

  if(m_dragged) {
    std::cout << "Eh? We are already dragging?" << std::endl;
#if 0
    m_board->m_drop_sprite = NamedSprite();
#endif
    cancelDragging(); //never remove implicitly a piece from the pool
  }

  int index = toLogical(pos);
  NamedSprite got = takeSpriteAt(index);
  if(!got)
    return;

  got.sprite()->hide();

  /* recreate the sprite, as "got" may be being animated */
  QPoint at = pos + this->pos() - m_board->pos() - QPoint(px.width(), px.height())/2;
  m_dragged = NamedSprite(  got.name(), SpritePtr(new Sprite(m_board->m_loader( got.name() ),
                                                             m_board->piecesGroup(),  at) ) );
  m_dragged.sprite()->raise();
  m_dragged.sprite()->show();
#if 0
  m_board->m_drop_sprite = m_dragged;
#endif
}

/*****************************************************************************************/
void PiecePool::onMouseMove(const QPoint& pos, int /*button*/) {
  if(m_dragged) {
    m_dragged.sprite()->moveTo(pos + this->pos() - m_board->pos()
                - QPoint(m_dragged.sprite()->pixmap().width(),
                         m_dragged.sprite()->pixmap().height() ) / 2 );
    m_board->draggingOn( 0/*BROKEN*/, m_dragged_index, pos + this->pos() - m_board->pos() );
  }
}
