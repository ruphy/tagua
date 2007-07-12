/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include <cmath>
#include <QTimer>
#include "global.h"
#include "piecegroup.h"
#include "pointconverter.h"
#include "animation.h"
#include "sprite.h"

using namespace boost;

//BEGIN PieceGroup

PieceGroup::PieceGroup(KGameCanvasAbstract* parent)
: ClickableCanvas(parent)
, m_flipped(false)
, m_square_size(0) {
  m_main_animation = new MainAnimation( 1.0 );
  settingsChanged();
}

PieceGroup::~PieceGroup()
{
  delete m_main_animation;
}

void PieceGroup::settingsChanged() {
  Settings s_anim = settings.group("animations");
  if (s_anim.flag("enabled", true)) {
    m_anim_fade = s_anim["fading"].flag("enabled", true);
    m_anim_movement = s_anim["movement"].flag("enabled", true);
  }
  else {
    m_anim_fade = false;
    m_anim_movement = false;
  }

  int speed = (s_anim["speed"] | 16);
  int smoothness = (s_anim["smoothness"] | 16);
  m_main_animation->setSpeed( 0.4*pow(10.0, speed/32.0) );
  m_main_animation->setDelay( int(70.0*pow(10.0, -smoothness/32.0)) );
}

void PieceGroup::onResize(int new_size, bool force_reload) {
  if(m_square_size == new_size && !force_reload)
    return;

  m_square_size = new_size;
  m_loader.setSize(m_square_size);
}

void PieceGroup::animatePiece(const shared_ptr<Sprite>& piece,
                                    const Point& to, double speed) {
  enqueue(
    m_anim_movement
    ? shared_ptr<Animation>(new MovementAnimation(piece, converter()->toReal(to), speed))
    : shared_ptr<Animation>(new InstantAnimation(piece, converter()->toReal(to)))
  );
}

void PieceGroup::enqueue(const shared_ptr<Animation>& anim) {
  m_main_animation->addAnimation(anim);
}

void PieceGroup::stopAnimations() {
  m_main_animation->stop();
}

void PieceGroup::finalizeAnimation(AnimationGroup* group) {
  delete group;
}

void PieceGroup::adjustSprite(const Point& p, bool smooth) {
  shared_ptr<Sprite> sprite = spriteAt(p);
  if (sprite) {
    if (smooth) {
      animatePiece(sprite, p, 1.0);
    }
    else {
      enqueue(shared_ptr<Animation>(new InstantAnimation(sprite, converter()->toReal(p))));
    }
  }
}

void PieceGroup::fadeIn(const Point& p) {
  shared_ptr<Sprite> sprite = spriteAt(p);
  if (sprite) {
    if(m_anim_fade)
      enqueue(shared_ptr<Animation>(new FadeAnimation(sprite, converter()->toReal(p), 0, 255)));
    else
      enqueue(shared_ptr<Animation>(new DropAnimation(sprite)));
  }
}

SpritePtr PieceGroup::createSprite(const QPixmap& pix, const Point& pos) {
  return SpritePtr(new Sprite(pix, piecesGroup(), converter()->toReal(pos)));
}

//END PieceGroup
