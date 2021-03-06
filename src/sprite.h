/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef SPRITE_H
#define SPRITE_H

#include "kgamecanvas.h"
#include "random.h"
#include <boost/weak_ptr.hpp>
#include <QPixmap>

class QPoint;
class QImage;
class Animation;
class FadeAnimation;
class SpriteExplosion;

#undef DEBUG_PIECE

/**
  * @class Sprite <sprite.h>
  * @brief The sprite of a piece.
  *
  * This class is a KGameCanvasPixmap that enables a few nifty
  * effects and keeps some piece-related information.
  */
class Sprite : public KGameCanvasPixmap {
private:
  /** the piece id (for convenience) */
  int m_id;

  /** the piece color (for convenience, could be -1) */
  int m_color;

  /** the piece type (for convenience, could be -1) */
  int m_type;

  QPixmap m_pixmap;

  /** the movement animation class */
  boost::weak_ptr<Animation> m_movement_animation;


  /** the step in the explostion */
  float m_explode_step;

  /** the explostion object */
  SpriteExplosion* m_explosion;

  /** rotation factor */
  float m_rotation;

  /** scaling factor */
  float m_scale;

  /** creates a new explosion object */
  SpriteExplosion* createExplosion(Random& random);

  /** painting implementation */
  virtual void paint(QPainter* p);

  /** returns the rectangle having to be painted */
  virtual QRect rect() const;

public:
  /** Constructor */
  Sprite(const QPixmap& pix, KGameCanvasAbstract* canvas, const QPoint& location);
  virtual ~Sprite();

  /** duplicates the piece */
  Sprite* duplicate() const;

  void setThumb(const QImage& thumb);
  void removeThumb();

  /** updates the pixmap */
  /* NOTE for paolo: why virtual? */
  virtual void setPixmap(const QPixmap&);

  /** set the movement animation */
  void setMovementAnimation(const boost::shared_ptr<Animation>& animation);

  /** returns the movement animation */
  boost::weak_ptr<Animation> movementAnimation() const;

//   void setFadeAnimation(const boost:shared_ptr<FadeAnimation>& animation);
//   boost:weak_ptr<FadeAnimation> fadeAnimation() const;

  /** set the explosion step (you have to call setupExplosion before) */
  void setExplosionStep(float f);

  /** set the rotations factor */
  void setRotation(float f);

  /** set the scaling factor */
  void setScale(float f);

  /** prepares a new explosion animation */
  void setupExplosion(Random& random);

#ifdef DEBUG_PIECE
  int m_dummy_opacity;
  bool m_dummy_visible;

  void update_from_dummy(){
    KGameCanvasItem::show();
    KGameCanvasItem::setOpacity(m_dummy_visible ? 64+m_dummy_opacity*3/8 : 64);
  }

  /* those should be enough, even if they are not virtual, as Piece is never casted IIRC */
  int opacity(){ return m_dummy_opacity; }
  int visible(){ return m_dummy_visible; }
  void setOpacity(int v){ m_dummy_opacity=v; update_from_dummy(); }
  void setVisible(bool v){ m_dummy_visible=v; update_from_dummy(); }
  void show(){ m_dummy_visible=true; update_from_dummy(); }
  void hide(){ m_dummy_visible=false; update_from_dummy(); }
#endif
};

#endif // SPRITE_H
