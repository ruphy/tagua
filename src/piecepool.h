/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECEPOOL_H
#define PIECEPOOL_H

#include <boost/shared_ptr.hpp>
#include "animationfactory.h"
#include "clickablecanvas.h"
#include "indexconverter.h"
#include "mainanimation.h"
#include "namedsprite.h"
#include "pixmaploader.h"

/**
  * @class PiecePool <piecepool.h>
  * @brief The class representing a crazyhouse-like piece pool.
  *
  * PiecePool is a canvas item attached to a Board that keeps a
  * stack of pieces and lets you drop them on the board, notifying
  * the board.
  */
class PiecePool : public ClickableCanvas
                , private IndexConverter {

private:
  int m_pool_num;

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

  const AnimationSettings& m_anim_settings;

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
  virtual QPoint toReal(int i) const;

  /** finds to which index corresponds the point p, or -1 if corresponds to none */
  virtual int toLogical(const QPoint& p) const;

  void animate(const Animate::Pool::Scheme& scheme, Animate::AnimationType type = Animate::Normal);
public:
  friend class GraphicalSystem;
  friend class ChessTable;

  /** Constructor, requires the board the pool will be attached to */
  PiecePool(int num, Board* b, const AnimationSettings&, KGameCanvasAbstract* parent);
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

  /** removes the sprite at the given index from the pool. */
  void removeSprite(int index);

  /** removes the sprite at the given index from the pool and returns it. */
  NamedSprite takeSprite(int index);



  /** sets the width of the grid (the fill will stay the same, and the
      grid height will be recalculated) */
  void setGridWidth(int w);

  /** the rect that will be covered by the pool */
  virtual QRect boardRect();

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
