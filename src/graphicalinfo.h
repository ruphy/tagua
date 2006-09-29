/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRAPHICALINFO_H
#define GRAPHICALINFO_H

#include <boost/shared_ptr.hpp>
#include <QObject>
#include "kboard.h"
#include "element.h"
#include "graphicalposition.h"

class ChessTable;
class Board;
class Animator;
class GraphicalPosition;
class UserEntity;
class GraphicalInfo;

class GraphicalInfoProxy : public QObject {
Q_OBJECT
  friend class GraphicalInfo;
  GraphicalInfo *m_info;
  GraphicalInfoProxy(GraphicalInfo *i) : QObject(), m_info(i) {}

public slots:
  void settingsChanged();
};

/**
  * @class GraphicalInfo <graphicalinfo.h>
  * @brief The graphical information about sprites and pieces.
  *
  * This class provides the variant's animator the facilites requires to add/remove
  * pieces to and from the board, keeping syncronized the sprites and the underlaying
  * board pieces.
  */
class GraphicalInfo : private GraphicalPosition {
public: // TODO: temporarily remove access limitations.
        // NOTE to Maurizio: why are getElement, setElement, etc... private?
  typedef boost::shared_ptr<PieceSprite> SpritePtr;

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

  /** signal proxy */
  GraphicalInfoProxy* m_proxy;

  /** @a GraphicalPosition interface function implementation */
  virtual int getIntSetting(const QString& key, int def_value) const;

  /** @a GraphicalPosition interface function implementation */
  virtual bool getBoolSetting(const QString& key, bool def_value) const;

  /** @a GraphicalPosition interface function implementation */
  virtual QString getStringSetting(const QString& key, const QString& def_value) const;

  /** @a GraphicalPosition interface function implementation */
  virtual SpritePtr getSprite(const Point& p) const;

  /** @a GraphicalPosition interface function implementation */
  virtual SpritePtr setPiece(const Point& p, AbstractPiece::Ptr e,
                              bool usedrop = false, bool show = true);

  /** @a GraphicalPosition interface function implementation */
  virtual Element getElement(const Point& p) const;

  /** @a GraphicalPosition interface function implementation */
  virtual void setElement(const Point& p, const Element& e);

  /** @a GraphicalPosition interface function implementation */
  virtual void removeElement(const Point& p);

  /** @a GraphicalPosition interface function implementation */
  virtual void updatePool(AbstractPosition::PoolPtr pool);

  /** @a GraphicalPosition interface function implementation */
  virtual void addTag(const QString& name, Point, bool over = false);

  /** @a GraphicalPosition interface function implementation */
  virtual void clearTags(const QString& name);

  /** @a GraphicalPosition interface function implementation */
  virtual void setTags(const QString& name, Point p1 = Point::invalid(), Point p2 = Point::invalid(),
                                      Point p3 = Point::invalid(), Point p4 = Point::invalid(),
                                      Point p5 = Point::invalid(), Point p6 = Point::invalid() );

  /** @a GraphicalPosition interface function implementation */
  virtual Point first() const;

  /** @a GraphicalPosition interface function implementation */
  virtual Point last() const;

  /** @a GraphicalPosition interface function implementation */
  virtual Point next(const Point& p) const;

  /** @a GraphicalPosition interface function implementation */
  virtual bool valid(const Point& p) const;


  /** @a GraphicalPosition interface function implementation */
  virtual bool consistent() const;

  /** internal function */
  void settingsChanged();

  /** internal function */
  virtual void addToPool(AbstractPiece::Ptr p, int n);

  /** internal function */
  virtual void removeFromPool(AbstractPiece::Ptr p, int n);

public:
  /** Constructor */
  GraphicalInfo(ChessTable* view, AbstractPosition::Ptr startingPosition,
                                              VariantInfo* variant);
  virtual ~GraphicalInfo();

  /** Sets the reference entity */
  void setup(const boost::shared_ptr<UserEntity>& entity);

  /** Goes forward playing a move, that has to be legal and checked */
  void forward(AbstractMove::Ptr move, AbstractPosition::Ptr pos,
               const boost::shared_ptr<PieceSprite>& = boost::shared_ptr<PieceSprite>());

  /** Goes back undoing a move, that has to be legal and checked */
  void back(AbstractMove::Ptr lastMove, AbstractMove::Ptr move, AbstractPosition::Ptr pos);

  /** Warps to a new position */
  void warp(AbstractMove::Ptr lastMove, AbstractPosition::Ptr pos);

  /** Adjusts a sprite to the correct position */
  void adjustSprite(const Point& p);

  /** Sets the current turn */
  void setTurn(int turn);

  /** Returns the position */
  AbstractPosition::Ptr position() const { return m_pos; }
};


#endif //GRAPHICALINFO_H
