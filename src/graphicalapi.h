/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRAPHICALAPI_H
#define GRAPHICALAPI_H

#include <boost/shared_ptr.hpp>
#include "kboard.h"
#include "pointconverter.h"
#include "sprite.h"

typedef boost::shared_ptr<class Sprite> SpritePtr;

/**
  * This class defines the interface that will be used by the animator to modify
  * kboard graphics.
  */
class GraphicalAPI {
public:
  virtual ~GraphicalAPI(){}

  /**
    * \return the current abstract position.
    */
  virtual const PointConverter* converter() const = 0;

  /**
    * \return the current abstract position.
    */
  virtual AbstractPosition::Ptr position() const = 0;

  /**
    * \return a sprite at the position \a index in the graphical pool.
    */
  virtual SpritePtr getSprite(const Point& p) = 0;

  /**
    * Removes a sprite at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual SpritePtr takeSprite(const Point& p) = 0;

  /**
    * Sets the sprite at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual SpritePtr setSprite(const Point& p, const AbstractPiece* piece, bool use_drop, bool show) = 0;

  /**
    * \return how many sprites are contained in the pool
    */
  virtual int poolSize(int pool) = 0;

  /**
    * \return the sprite at the position \a index in the graphical pool.
    */
  virtual SpritePtr getPoolSprite(int pool, int index) = 0;

  /**
    * Removes the sprite at the position \a index in the graphical pool.
    * \return the removed sprite.
    */
  virtual SpritePtr takePoolSprite(int pool, int index) = 0;

  /**
    * Inserts a sprite at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual SpritePtr insertPoolSprite(int pool, int index, const AbstractPiece* piece) = 0;
};

#endif //GRAPHICALAPI_H
