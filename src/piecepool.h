/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECEPOOL_H
#define PIECEPOOL_H

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
class PiecePool : public ClickableCanvas {

private:
  /** displayed m_sprites */
  std::vector<NamedSprite>   m_sprites;

  /** refrence board */
  class Board* m_board;

  /** true if flipped */
  bool m_flipped;

  /** size if a square */
  int m_square_size;

  /** the width of the pool (ie how many pieces) */
  int m_width;

  /** loader class, to load pieces */
  PixmapLoader m_loader;

  /** the piece that is being dragged, if any */
  NamedSprite m_dragged;

  /** the index of the piece being dragged */
  int m_dragged_index;

  /** main animation structure */
  MainAnimation* m_main_animation;

  /** internal function, resizes the grid vector to hold x pieces */
  void setFill(int x);

  /** removes the drag putting it back together with his friends in the pool.
      if fadeOff is true the current drag will fade off while a new piece will
      fade in the pool, while if fadeOff is false the fade off is not done.
      fadeOff will typically be false if the piece sprite has been used in the
      board and we don't want a clone that is fading off */
  void cancelDragging(bool fadeOff = true);

  /** takes the named sprite */
  NamedSprite takeSpriteAt(int i);

  /** converts an index to the upper left point of the corresponding square */
  QPoint toReal(int i);

  /** finds to which index corresponds the point p, or -1 if corresponds to none */
  int toLogical(const QPoint& p);

public:
  friend class GraphicalSystem;
  friend class ChessTable;

  /** Constructor, requires the board the pool will be attached to */
  PiecePool(Board* b, KGameCanvasAbstract* parent);
  ~PiecePool();

  /** returns the sprite loader */
  PixmapLoader* loader() { return &m_loader; }

  /** returns the sprite loader */
  const PixmapLoader* loader() const { return &m_loader; }

  /** returns the flipped value */
  bool flipped() const { return m_flipped; }

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

  /** piecesGroup overload */
  virtual KGameCanvasAbstract* piecesGroup();

  /** the rect that will be covered by the pool */
  virtual QRect boardRect() { return QRect(pos(), QSize(m_square_size*m_width,
                                  (m_flipped?-1:1)*m_square_size*((m_sprites.size()+m_width-1)/m_width)) ); }

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

#endif //PIECEPOOL_H
