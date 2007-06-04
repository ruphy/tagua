/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRAPHICALPOSITION_H
#define GRAPHICALPOSITION_H

#include <boost/shared_ptr.hpp>
#include "grid.h"

class PieceSprite;
namespace Canvas { class Abstract; }

/**
  * @class GraphicalPosition <graphicalposition.h>
  * @brief The graphical position interface exported to the variant specific code.
  *
  * This class provides the variant's animator the facilites requires to add/remove
  * pieces to and from the board, creating and destroying the related sprites.
  */
class GraphicalPosition {
protected:
  virtual ~GraphicalPosition() {}

public:
  typedef boost::shared_ptr<PieceSprite> SpritePtr;

  /** Returns a settings relative to this position */
  virtual int getIntSetting(const QString& key, int def_value) const = 0;

  /** Returns a settings relative to this position */
  virtual bool getBoolSetting(const QString& key, bool def_value) const = 0;

  /** Returns a settings relative to this position */
  virtual QString getStringSetting(const QString& key, const QString& def_value) const = 0;

  /** Returns the piece sprite at the given position */
  virtual SpritePtr getSprite(const Point& p) const = 0;

  /** Puts a piece at the given position, creating a piece sprite if needed.
     If @p usedrop is true, and a piece have just been dropped, it will be
     used if possible (it has the correct id). The @p show parameter controls
     if the new sprite will be showed. */
  virtual SpritePtr setPiece(const Point& p, AbstractPiece::Ptr e,
                             bool usedrop = false, bool show = false) = 0;

  /** Gets the piece/sprite at the given point */
  virtual Element getElement(const Point& p) const = 0;

  /** Sets the piece/sprite at the given point */
  virtual void setElement(const Point& p, const Element& e) = 0;

  /** Removes the piece/sprite at the given point */
  virtual void removeElement(const Point& p) = 0;

  /** Updates the pool accordingly to the given one. */
  virtual void updatePool(AbstractPosition::PoolPtr pool) = 0;

  /* NOTE: Question: should tags be piecesprites, this function returning the sprite? */
  /** Adds a tag to the board */
  virtual void addTag(const QString& name, Point, bool over=false) = 0;

  /** Removes all tags with @p name */
  virtual void clearTags(const QString& name) = 0;

  /** Clears all tags with @p name, and creates the given ones */
  virtual void setTags(const QString& name, Point p1 = Point::invalid(), Point p2 = Point::invalid(),
                                      Point p3 = Point::invalid(), Point p4 = Point::invalid(),
                                      Point p5 = Point::invalid(), Point p6 = Point::invalid() ) = 0;

  /** Returns the first point, to enumerate the squares */
  virtual Point first() const = 0;

  /** Returns the last point, to enumerate the squares */
  virtual Point last() const = 0;

  /** Returns the next point, to enumerate the squares */
  virtual Point next(const Point& p) const = 0;

  /** Returns true if the point is not out of the board */
  virtual bool valid(const Point& p) const = 0;

  /** Checks if the piece/sprite data match (for debugging) */
  virtual bool consistent() const = 0;
};

#endif // GRAPHICALPOSITION_H
