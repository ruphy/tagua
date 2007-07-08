/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRAPHICALSYSTEM_H
#define GRAPHICALSYSTEM_H

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "graphicalapi.h"

class ChessTable;
class Board;
class UserEntity;

class GraphicalSystem : public QObject, private GraphicalAPI {
Q_OBJECT

public:

  /** The current chess table */
  ChessTable* m_view;

  /** The current board */
  Board* m_board;

  /** The position data */
  AbstractPosition::Ptr m_pos;

  /** The variant specific animator */
  AbstractAnimator::Ptr m_animator;

  /** The current variant */
  VariantInfo* m_variant;

  /** @a GraphicalPosition interface function implementation */
//   virtual void addTag(const QString& name, Point, bool over = false);

  /** @a GraphicalPosition interface function implementation */
//   virtual void clearTags(const QString& name);

  /** @a GraphicalPosition interface function implementation */
//   virtual void setTags(const QString& name, Point p1 = Point::invalid(), Point p2 = Point::invalid(),
//                                       Point p3 = Point::invalid(), Point p4 = Point::invalid(),
//                                       Point p5 = Point::invalid(), Point p6 = Point::invalid() );

private slots:
  /** internal function to listen at setting changes */
  void settingsChanged();

  /**
    * \return the current abstract position. (interface for GraphicalAPI)
    */
  virtual const PointConverter* converter() const;

  /**
    * \return the current abstract position. (interface for GraphicalAPI)
    */
  virtual AbstractPosition::Ptr position() const;

  /**
    * \return a sprite at the position \a index in the graphical pool. (interface for GraphicalAPI)
    */
  virtual NamedSprite getSprite(const Point& p);

  /**
    * Removes a sprite at the position \a index in the graphical pool. (interface for GraphicalAPI)
    * \return the newly created sprite.
    */
  virtual NamedSprite takeSprite(const Point& p);

  /**
    * Sets the piece at the position \a index in the graphical pool. (interface for GraphicalAPI)
    * \return the newly created sprite.
    */
  virtual NamedSprite setPiece(const Point& p, const AbstractPiece* piece, bool use_drop, bool show);

  /**
    * Sets the sprite at the position \a index in the graphical pool. (interface for GraphicalAPI)
    * \return the newly created sprite.
    */
  virtual void setSprite(const Point& p, const NamedSprite& sprite);

  /**
    * \return how many sprites are contained in the pool
    */
  virtual int poolSize(int pool);

  /**
    * \return the sprite at the position \a index in the graphical pool. (interface for GraphicalAPI)
    */
  virtual NamedSprite getPoolSprite(int pool, int index);

  /**
    * Removes the sprite at the position \a index in the graphical pool. (interface for GraphicalAPI)
    * \return the removed sprite.
    */
  virtual NamedSprite takePoolSprite(int pool, int index);

  /**
    * Inserts a sprite at the position \a index in the graphical pool. (interface for GraphicalAPI)
    * \return the newly created sprite.
    */
  virtual NamedSprite insertPoolPiece(int pool, int index, const AbstractPiece* piece);


public:
  /** Constructor */
  GraphicalSystem(ChessTable* view, AbstractPosition::Ptr startingPosition,
                                              VariantInfo* variant);
  virtual ~GraphicalSystem();

  /** Sets the reference entity */
  void setup(const boost::shared_ptr<UserEntity>& entity);

  /** Goes forward playing a move, that has to be legal and checked */
  void forward(const AbstractMove::Ptr& move,
               const AbstractPosition::Ptr& pos,
               const SpritePtr& = SpritePtr());

  /** Goes back undoing a move, that has to be legal and checked */
  void back(const AbstractMove::Ptr& lastMove,
            const AbstractMove::Ptr& move,
            const AbstractPosition::Ptr& pos);

  /** Warps to a new position */
  void warp(const AbstractMove::Ptr& lastMove,
            const AbstractPosition::Ptr& pos);

  /** Adjusts a sprite to the correct position */
  void adjustSprite(const Point& p);

  /** Sets the current turn */
  void setTurn(int turn);
};


#endif //GRAPHICALSYSTEM_H
