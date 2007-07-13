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
PiecePool::PiecePool(int num, Board* b, KGameCanvasAbstract* parent)
: ClickableCanvas(parent)
, m_pool_num(num)
, m_board(b)
, m_flipped(false)
, m_square_size(0)
, m_width(1)
, m_dragged_index(-1) {
  m_main_animation = new MainAnimation( 1.0 );
  setGridWidth(1);
}

/*****************************************************************************************/
PiecePool::~PiecePool() {
  delete m_main_animation;
}

/*****************************************************************************************/
QPoint PiecePool::toReal(int i) {
  int x = i%m_width;
  int y = i/m_width;
  if(y&1)
    x = m_width-1-x;
  if(m_flipped)
    y = -1-y;

  return QPoint(m_square_size*x, m_square_size*y);
}

/*****************************************************************************************/
int PiecePool::toLogical(const QPoint& p) {
  int x = p.x()/m_square_size;
  int y = p.y()/m_square_size;

  if(x<0 || x>= m_width)
    return -1;
  if(m_flipped)
    y = -1-y;
  if(y&1)
    x = m_width-1-x;

  int retv = y*m_width + x;
  if(retv < 0 || retv >= (int)m_sprites.size())
    return -1;
  return retv;
}

/*****************************************************************************************/
void PiecePool::settingsChanged() {
  //PieceGroup::settingsChanged();
}

/*****************************************************************************************/
void PiecePool::setGridWidth(int w) {
  m_width = w;
}

/*****************************************************************************************/
int PiecePool::fill() {
  return m_sprites.size();
}

/*****************************************************************************************/
void PiecePool::clear() {
  m_sprites.clear();
}

/*****************************************************************************************/
void PiecePool::insertSprite(int index, const NamedSprite& nsprite) {
  if(m_dragged && index > m_dragged_index)
    index--;

  if(index < 0 || index > fill() ) {
    ERROR("invalid index " << index); TRAP();
    return;
  }

  m_sprites.resize(m_sprites.size()+1);

  for(int i = m_sprites.size()-1; i > index; i--) {
    double speed = (1.0+1.0/(i - index + 1))*0.4;
    m_sprites[i] = m_sprites[i-1];
    m_sprites[i].sprite()->moveTo(toReal(i)); //BROKEN animate to that point?
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

  if(index < 0 || index >= (int)m_sprites.size() ) {
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

  if(index < 0 || index >= (int)m_sprites.size() ) {
    ERROR("invalid index " << index);
    return NamedSprite();
  }

  return takeSpriteAt(index);
}

/*****************************************************************************************/
NamedSprite PiecePool::takeSpriteAt(int index) {
  if(index < 0 || index >= (int)m_sprites.size() ) {
    ERROR("invalid index " << index); TRAP();
    return NamedSprite();
  }

  NamedSprite retv = m_sprites[index];
  if(!retv)
    return NamedSprite();

  for(int i = index; i < (int)m_sprites.size()-1; i++) {
    double speed = (1.0+1.0/(i - index + 1))*0.4;
    m_sprites[i] = m_sprites[i+1];
    m_sprites[i].sprite()->moveTo(toReal(i)); //BROKEN animate to that point?
  }
  m_sprites.resize(m_sprites.size()-1);

  return retv;
}

/*****************************************************************************************/
KGameCanvasAbstract* PiecePool::piecesGroup() {
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
    if(1/*BROKEN m_anim_fade*/)
      m_main_animation->addAnimation( AnimationPtr(new FadeAnimation(phantom, phantom->pos(), 255, 0)) );
    else
      m_main_animation->addAnimation( AnimationPtr(new CaptureAnimation(phantom)) );
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

  for(int i=0;i<(int)m_sprites.size(); i++)
  if(1/*BROKEN m_anim_movement*/) {
    m_main_animation->addAnimation(AnimationPtr(new InstantAnimation(m_sprites[i].sprite(),
                                              m_sprites[i].sprite()->pos() - deltapos)));
    m_main_animation->addAnimation(AnimationPtr(new MovementAnimation(m_sprites[i].sprite(),
                                              toReal(i), 1.0)));
  }
  else
    m_main_animation->addAnimation(AnimationPtr(new InstantAnimation(m_sprites[i].sprite(),
                                              toReal(i))));
}

/*****************************************************************************************/
void PiecePool::onResize(int new_size, bool force_reload) {
  if(m_square_size == new_size && !force_reload)
    return;

  m_square_size = new_size;
  m_loader.setSize(m_square_size);

  // update the sprites
  for (int i=0;i<(int)m_sprites.size(); i++) {
    m_sprites[i].sprite()->setPixmap( m_loader( m_sprites[i].name() ) );
    m_main_animation->addAnimation(AnimationPtr(new InstantAnimation(m_sprites[i].sprite(), toReal(i))));
  }
}

/*****************************************************************************************/
void PiecePool::onMouseRelease(const QPoint& pos, int button) {
  if (button != Qt::LeftButton || !m_dragged)
    return;

  m_board->m_dropped_pool = m_pool_num;
  m_board->m_dropped_index = m_dragged_index;

  /* did the board take this sprite? */
  m_board->dropOn( m_pool_num, m_dragged_index, pos + this->pos() - m_board->pos() );

  m_board->m_dropped_pool = -1;
  m_board->m_dropped_index = -1;

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
  QPixmap px = m_board->m_loader( got.name() );
  QPoint at = pos + this->pos() - m_board->pos() - QPoint(px.width(), px.height())/2;
  m_dragged = NamedSprite(  got.name(), SpritePtr(new Sprite(px, m_board->piecesGroup(),  at) ) );
  m_dragged.sprite()->raise();
  m_dragged.sprite()->show();
  m_dragged_index = index;
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
    m_board->draggingOn( m_pool_num, m_dragged_index, pos + this->pos() - m_board->pos() );
  }
}
