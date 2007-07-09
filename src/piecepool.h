/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef __PIECEPOOL_H__
#define __PIECEPOOL_H__

#include <boost/shared_ptr.hpp>
#include "piecegroup.h"
#include "namedsprite.h"

/**
  * @class PiecePool <piecepool.h>
  * @brief The class representing a crazyhouse-like piece pool.
  *
  * PiecePool is a canvas item attached to a Board that keeps a
  * stack of pieces and lets you drop them on the board, notifying
  * the board.
  */
class PiecePool : public PieceGroup {
public:
  typedef Grid<NamedSprite> PieceGrid;

private:
  /** displayed m_sprites */
  PieceGrid   m_sprites;

  /** refrence board */
  class Board* m_board;

  /** the piece that is being dragged, if any */
  NamedSprite m_dragged;

  /** the index of the piece being dragged */
  int m_dragged_index;

  /** the number of pieces on the pool */
  int m_fill;

  /** internal function, resizes the grid vector to hold x pieces */
  void setFill(int x);

  /** redefinition of PointConverter::flipPoint
      if the PiecePool is flipped it will be drawn using the position as
      lower bound. Further, the points are ordered in a snake-like way */
  virtual Point flipPoint(const Point& p) const; //custom flip function

  /** removes the drag putting it back together with his friends in the pool.
      if fadeOff is true the current drag will fade off while a new piece will
      fade in the pool, while if fadeOff is false the fade off is not done.
      fadeOff will typically be false if the piece sprite has been used in the
      board and we don't want a clone that is fading off */
  void cancelDragging(bool fadeOff = true);

  /** this internal function updates the sprite images after the board has been resized  */
  void updateSprites();

  /** fetch the sprite */
  boost::shared_ptr<Sprite> spriteAt(const Point& p) { return m_sprites[p].sprite(); }

  /** takes the named sprite */
  NamedSprite takeNamedSprite(const Point& p);

public:
  friend class GraphicalSystem;
  friend class ChessTable;

  /** Constructor, requires the board the pool will be attached to */
  PiecePool(Board* b, Canvas::Abstract* parent);

  ~PiecePool();


  /** returns the number of pieces in the pool */
  int fill();

  /** removes all the pieces */
  void clear();

  /** adds a sprite to the pool */
  void insertSprite(int index, const NamedSprite& sprite);

  /** \return the sprite at the given index. */
  NamedSprite getSprite(int index);

  /** removes the sprite at the given index from the pool and returns it. */
  NamedSprite takeSprite(int index);



  /** sets the width of the grid (the fill will stay the same, and the
      grid height will be recalculated) */
  void setGridWidth(int w);

  /** \return the size of the grid */
  virtual Point gridSize() const { return m_sprites.getSize(); }

  /** piecesGroup overload */
  virtual Canvas::Abstract* piecesGroup();

  /** the rect that will be covered by the pool */
  virtual QRect boardRect() { return QRect(pos(), QSize(m_square_size*gridSize().x,
                                  (flipped()?-1:1)*m_square_size*gridSize().y)); }

  /** flips and moves the pieces in the pool at the same time */
  void flipAndMoveBy(QPoint p);

  /** mouse release event  */
  virtual void onMouseRelease(const QPoint& pos, int button);

  /** mouse press event  */
  virtual void onMousePress(const QPoint& pos, int button);

  /** mouse move event  */
  virtual void onMouseMove(const QPoint& pos, int button);

  /** resize event (new_size is the new size of a SQUARE) */
  virtual void onResize(int new_size, bool force_reload = false);

  /** changed settings handler */
  virtual void settingsChanged();
};

#endif //__PIECEPOOL_H__
