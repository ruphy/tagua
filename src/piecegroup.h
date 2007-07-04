/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECEGROUP_H
#define PIECEGROUP_H

#include <boost/shared_ptr.hpp>
#include "canvas/group.h"
#include "point.h"
#include "grid.h"
#include "animation.h"
#include "sprite.h"
#include "mainanimation.h"
#include "pointconverter.h"
#include "spriteloader.h"
#include "clickablecanvas.h"

class MainAnimation;
class PointConverter;
class SpriteLoader;

//BEGIN PieceGroup

/**
  * @class PieceGroup <piecegroup.h>
  * @brief The abstract base class for all widgets that draw pieces.
  *
  * PieceGroup is a general abstract widget that can be subclassed to
  * implement a rectangular widget with piece m_sprites on it.
  * It uses the method template pattern to provide a flexible yet comfortable
  * api for subclassing.
  */
class PieceGroup : public ClickableCanvas, protected PointConverter {
public:

protected:
  /** fading animations enabled */
  bool        m_anim_fade;

  /** movement animations enabled */
  bool        m_anim_movement;

  /** true if the board is flipped */
  bool        m_flipped;

  /** size of a square in points */
  int         m_square_size;

  /** loader class, to load pieces */
  SpriteLoader m_loader;

  /** main animation structure */
  MainAnimation* m_main_animation;

  /** enqueue an animation */
  void enqueue(const boost::shared_ptr<Animation>&);

  /** stops all animations */
  void stopAnimations();

  /** finalizes an animaiton group */
  void finalizeAnimation(AnimationGroup*);

  /** ajdusts the sprite's position */
  void adjustSprite(const Point&, bool smooth = false);

  /** enqueue a movement animation (convenience function) */
  void animatePiece(const boost::shared_ptr<Sprite>& piece,
                                  const Point& to, double speed);

  /** fades in the sprite at a given point */
  void fadeIn(const Point&);

  /** returns the sprite at p */
  virtual boost::shared_ptr<Sprite> spriteAt(const Point&) = 0;

public:
  PieceGroup(Canvas::Abstract* parent);
  virtual ~PieceGroup();

  /** returns the point converter class */
  PointConverter* converter() { return static_cast<PointConverter*>(this); }

  /** returns the point converter class */
  const PointConverter* converter() const { return static_cast<const PointConverter*>(this); }

  /** returns the sprite loader */
  SpriteLoader* loader() { return &m_loader; }

  /** returns the sprite loader */
  const SpriteLoader* loader() const { return &m_loader; }

  /** returns the flipped value */
  virtual bool  flipped() const { return m_flipped; }

  /** returns the size of a square */
  virtual int   squareSize() const { return m_square_size; }

  /** returns the size of the grid */
  virtual Point gridSize() const = 0;

  /** returns the center of a square */
  QPoint squareCenter(const QPoint& point) {
          return point - QPoint(m_square_size, m_square_size) / 2;
  }

  /** x size (deprecated, use boardRect instead) */
  int    boardSizeX() { return m_square_size*gridSize().x; }

  /** y size (deprecated, use boardRect instead) */
  int    boardSizeY() { return m_square_size*gridSize().y; }

  /** returns the area covered by the piece grid */
  virtual QRect boardRect() { return QRect(pos(), QSize(m_square_size*gridSize().x,
                                                 m_square_size*gridSize().y)); }

  /** returns the group that contains the pieces (override this) */
  virtual Canvas::Abstract* piecesGroup() = 0;

  /** resize event handler (updates the sprites and the square size) */
  virtual void onResize(int new_size, bool force_reload = false);

  /** changed settings handler */
  virtual void settingsChanged();
  
  /** create a sprite from a pixmap */
  virtual boost::shared_ptr<Sprite> createSprite(const QPixmap& pix, const Point& pos);
};

//END PieceGroup

#endif //PIECEGROUP_H
