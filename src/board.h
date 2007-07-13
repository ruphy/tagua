/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef BOARD_H
#define BOARD_H

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <QObject>
#include <QFont>
#include <boost/shared_ptr.hpp>
#include "kgamecanvas.h"
#include "entities/userentity.h"
#include "common.h"
#include "grid.h"
#include "usermove.h"
#include "piecegroup.h"
#include "namedsprite.h"

class DragInfo;
class Animation;
class UserEntity;

class BoardTags;
typedef boost::shared_ptr<BoardTags> BoardTagsPtr;

/**
  * @class Board <board.h>
  * @brief Board visualization class
  *
  * This class is a KGameCanvasItem that displaying a board filled with pieces.
  * You can set custom tags for each square.
  * @sa Table, @sa PiecePool, @sa Sprite
  */
class Board : public QObject, public PieceGroup {
  Q_OBJECT

public:
  friend class GraphicalSystem;
  friend class PiecePool;
  typedef Grid<NamedSprite> PieceGrid;

private:
  class DragInfo {
  public:
    static const int DRAG_THRESHOLD = 100; // pixels ^ 2
    boost::shared_ptr<Sprite> sprite;
    Point from; // logical coordinates
    QPoint real; /// real starting point, used to honor drag threshold
    bool dragging;
    bool dragStarted;
    bool droppedOut;
    InteractionType action;

    DragInfo(Point from, const QPoint& real,
             const boost::shared_ptr<Sprite>& sprite,
             InteractionType action)
    : sprite(sprite)
    , from(from)
    , real(real)
    , dragging(false)
    , dragStarted(false)
    , droppedOut(false)
    , action(action) {}
  };

  typedef boost::shared_ptr<DragInfo> DragInfoPtr;


  /** the piece being dragged */
  DragInfoPtr m_drag_info;

  /** displayed m_sprites */
  PieceGrid   m_sprites;

#if 0
  /** used by a PiecePool to make available the piece that is being dropped
     on the board to the GraphicalInfo and the variant-specific animator */
  NamedSprite m_drop_sprite;
#endif

  /** the visual move hint */
  NamedSprite m_hinting;
  Point m_hinting_pos;

  /** the canvas group that holds the pieces */
  KGameCanvasGroup *m_pieces_group;

  /** the canvas item that holds the background of the board */
  KGameCanvasGroup *m_canvas_background;

  /** the canvas item that holds board border */
  KGameCanvasGroup *m_canvas_border;

  /** all the tags on the board */
  BoardTagsPtr m_tags;

  /** the @a PixmapLoader used to create tags */
  PixmapLoader m_tags_loader;

  /** the entity responsible of all the interctions with the board */
  boost::weak_ptr<UserEntity> m_entity;

  /** The currently selected square */
  Point selection;

  /** The reviously selected square */
  Point lastSelection;

  /** premove */
  Point m_premove_from;
  Point m_premove_to;

  /** ascent of the font used for the border */
  int m_border_asc;

  /** show the border if true */
  bool m_show_border;

  QColor m_border_color;

  QColor m_border_text_color;

  QFont m_border_font;

  QStringList m_border_coords;

  int m_dropped_pool;
  int m_dropped_index;

  /** the text items for the border */
  std::vector<boost::shared_ptr<KGameCanvasItem> > m_border_items;

  /** this internal function updates the background after the board has been resized  */
  void updateBackground();

  /** this internal function updates the border after the board has been resized  */
  void updateBorder();

  /** this internal function creates a new border, if needed */
  void recreateBorder();

  void updateHinting(Point pt, AbstractPiece::Ptr piece);

  /** this internal function updates the sprites after the board has been resized  */
  void updateSprites();

  /** this internal function updates the tags after the board has been resized  */
  void updateTags();

  void mySettingsChanged();

  void setPremove(const NormalUserMove& move);
  void setPremove(const DropUserMove& move);
  void setPremove(const class Premove& move);
  void updatePremove();
  void setSelection(const Point& p);

  QRect computeRect(Point) const;
  QRegion computeRegion(Point) const;
  QRect squareRect(int x, int y) const;

  /** this function tries to notify the move to the entity */
  bool doMove(const NormalUserMove&);

  /** fetch the sprite */
  boost::shared_ptr<Sprite> spriteAt(const Point& p) { return m_sprites[p].sprite(); }

public:
  /** constructor, requires the canvas parent */
  Board(KGameCanvasAbstract* parent);
  ~Board();

  /** set the board flipping */
  void flip(bool flipped);

  /** flips the board */
  inline void flip() { flip(!m_flipped); }

  /** adds a tag with name name, the tag will stay over the pieces if over is true */
  boost::shared_ptr<KGameCanvasPixmap> addTag(const QString& name, Point at, bool over=false);

  /** clears the tags with name name */
  void clearTags(const QString& name);

  /** clears all the tags */
  void clearTags();

  /** clear all the tags twith name name, than sets them to the valid given ones */
  void setTags(const QString& name, Point p1 = Point::invalid(), Point p2 = Point::invalid(),
                                    Point p3 = Point::invalid(), Point p4 = Point::invalid(),
                                    Point p5 = Point::invalid(), Point p6 = Point::invalid() );

  /** returns a reference to the loader used to load tag pixmaps */
  PixmapLoader* tagsLoader() { return &m_tags_loader; }


  void cancelPremove();
  void cancelSelection();

  /** recreates the board underlying grid  */
  void createGrid(Point p, const QStringList& border_coords);


  /** sets the controlling entity */
  inline void setEntity(const boost::shared_ptr<UserEntity>& entity) { m_entity = entity; }


  /** Notifies to the board that a certain piece is being dragged over the board */
  void draggingOn(int pool, int index, const QPoint& p);

  /** Executes a drop. This function id typically called by by a PiecePool */
  bool dropOn(int pool, int index, const QPoint& point);

  /** returns the size of the grid */
  virtual Point gridSize() const { return m_sprites.getSize(); }

  /** return the group that contains the pieces */
  virtual KGameCanvasAbstract* piecesGroup() { return m_pieces_group; }

  /** mouse release event */
  virtual void onMouseRelease(const QPoint& pos, int button);

  /** mouse press event */
  virtual void onMousePress(const QPoint& pos, int button);

  /** mouse move event */
  virtual void onMouseMove(const QPoint& pos, int button);

  /** mouse leave event */
  virtual void onMouseLeave();

  /** resize event (new_size is the new size of a SQUARE) */
  virtual void onResize(int new_size, bool force_reload = false);

  /** the position changed (will update the move highlighting) */
  virtual void onPositionChanged();

  /** changed settings handler */
  virtual void settingsChanged();

  /** resets all tags and stops all animations */
  void reset();

signals:
  void error(ErrorCode code);
};

#endif //BOARD_H
