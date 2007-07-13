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

#include <vector>
#include <boost/shared_ptr.hpp>
#include "kboard.h"
#include "pointconverter.h"
#include "namedsprite.h"
#include "fwd.h"

typedef boost::shared_ptr<class Sprite> SpritePtr;

/**
  * This class defines the interface that will be used by the animator to modify
  * kboard graphics.
  */
class GraphicalAPI {
public:
  enum AnimationType {
    Normal,
    Instant
  };

  virtual ~GraphicalAPI() { }

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
  virtual NamedSprite getSprite(const Point& p) = 0;

  /**
    * Removes a sprite at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual NamedSprite takeSprite(const Point& p) = 0;

  /**
    * Sets the piece at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual NamedSprite setPiece(const Point& p, const AbstractPiece* piece, bool show) = 0;

	/**
	  *  Create a new piece, but do not add it to the graphical system.
	  * \return the newly created sprite.
	  */
  virtual NamedSprite createPiece(const Point& p, const AbstractPiece* piece,  bool show) = 0;

  /**
    * Sets the sprite at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual void setSprite(const Point& p, const NamedSprite& sprite) = 0;

  /**
    * \return how many sprites are contained in the pool
    */
  virtual int poolSize(int pool) = 0;

  /**
    * \return the sprite at the position \a index in the graphical pool.
    */
  virtual NamedSprite getPoolSprite(int pool, int index) = 0;

  /**
    * Removes the sprite at the position \a index in the graphical pool.
    * \return the removed sprite.
    */
  virtual NamedSprite takePoolSprite(int pool, int index) = 0;

  /**
    * Inserts a sprite at the position \a index in the graphical pool.
    * \return the newly created sprite.
    */
  virtual NamedSprite insertPoolPiece(int pool, int index, const AbstractPiece* piece) = 0;

  /**
    * \return the piece of the pool that has been dropped, or (-1,-1)
    */
  virtual std::pair<int, int> droppedPoolPiece() = 0;

  /**
  	* Create a movement animation.
  	* \param sprite The sprite to be animated.
  	* \param to The destination square.
  	* \return A newly created animation moving \a sprite between the specified squares.
  	*/
  virtual AnimationPtr moveAnimation(const NamedSprite& sprite, const Point& to, AnimationType type) = 0;

  /**
  	* Create an appear animation.
  	* \param sprite The sprite to be shown.
  	* \return A newly created animation showing \a sprite.
  	*/
  virtual AnimationPtr appearAnimation(const NamedSprite& sprite, AnimationType type) = 0;

	/**
		* Create a disappear animation.
  	* \param sprite The sprite to be hidden.
  	* \return A newly created animation hiding \a sprite.
		* \sa appearAnimation.
		*/
  virtual AnimationPtr disappearAnimation(const NamedSprite& sprite, AnimationType type) = 0;

	/**
		* Create a destruction animation.
		* \param sprite The sprite to be destroyed.
		* \return A newly created animation destroying \a sprite.
		*/
  virtual AnimationPtr destroyAnimation(const NamedSprite& sprite, AnimationType type) = 0;

	/**
		* Create a morphing animation.
		* \param sprite The sprite to be morphed.
		* \param new_sprite The final appearance of the morphed sprite.
		* \return A newly created morphing animation from \a sprite to \a new_sprite
		*/
  virtual AnimationPtr morphAnimation(const NamedSprite& sprite, const NamedSprite& new_sprite, AnimationType) = 0;
};

#endif //GRAPHICALAPI_H
