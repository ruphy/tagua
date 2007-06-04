/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LOWLEVEL_H
#define LOWLEVEL_H

/**
  * @file kboard.h
  * @brief Low level abstract classes used by the interface framework.
  *
  * This file includes definitions for abstract classes used from within
  * the interface framework to access variant details using dynamic time
  * polymorphism.
  *
  * One of the big problems with this approach is what I call 'lack of
  * functional dependency support' for runtime polymorphism.
  * What is missing is the ability to override a virtual member function
  * in a derived class changing its signature. Or better, it is possible
  * to only change its return type with a covariant type, while it would
  * often be useful to change any of its argument types with covariant or
  * even controvariant types.
  * For controvariant types, I foresee no problems, and all type checking
  * can be done statically (of course the method has to be dynamically
  * bound) as they are now.
  * When using covariant types, the compiler could insert appropriate
  * dynamic cast's and type checks in the calling code, when there's no
  * way to know the exact type of an argument at compile time.
  *
  * The file @ref highlevel.h tries to address this problem doing part
  * of the compiler's work. One exception is that type checking code
  * is inserted in a method wrapper, instead of the calling place, so
  * even calling a method of a noncasted instance would cause some
  * upcast / downcast overhead; this is negligible, though, because
  * that scenario is rather unlikely, since wrapped classes are always
  * kept in variables of abstract types.
  */

#include <map>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QStringList>
#include "point.h"
#include "usermove.h"
#include "index.h"
#include "option.h"
#include "decoratedmove.h"
#include "kboard_fwd.h"

/**
  * @brief A superclass for all the piece classes.
  */
class AbstractPiece {
public:
  typedef boost::shared_ptr<AbstractPiece> Ptr;
  virtual ~AbstractPiece() { }

  /**
    * Piece equality. Return false if the second pointer
    * is null.
    */
  virtual bool equals(AbstractPiece::Ptr other) const = 0;

  /**
    * Piece compare. Return true if less than other.
    */
  virtual bool less(AbstractPiece::Ptr other) const = 0;

  /**
    * Return the piece type id. It should be a nonnegative
    * number, or -1 if the type concept for this piece
    * is not applicable.
    */
  virtual int type() const = 0;

  /**
    * Return the piece color id. It should be a nonnegative
    * number, or -1 if the color concept for this piece
    * is not applicable.
    */
  virtual int color() const = 0;

  /**
    * Return a unique id for the piece
    * (type, color) pair. Used to store
    * pixmaps.
    */
//  virtual int id() const = 0;

  /**
    * Return a unique key for the piece.
    * Used to store pixmaps.
    */
  virtual QString name() const = 0;

  /**
    * Return the symbol used to identify the piece
    * in a move.
    */
//    virtual QString typeSymbol() const = 0;

  /**
    * Return the name used to load the
    * piece pixmap.
    * OBSOLETE
    */
//   virtual QString name() const = 0;

  /**
    * Create a deep copy of the piece.
    */
  virtual AbstractPiece::Ptr clone() const = 0;
};

/**
  * @brief A class to compare two pieces.
  */
class AbstractPieceComparer {
public:
  bool operator()(AbstractPiece::Ptr x, AbstractPiece::Ptr y) {
    return x->less(y);
  }
};

/**
  * @brief A superclass for all the move classes.
  *
  * An abstract move is simply something that can serialize
  * itself either as SAN notation, or as coordinate notation.
  */
class AbstractMove {
public:
  typedef boost::shared_ptr<AbstractMove> Ptr;
  virtual ~AbstractMove() { }

  /**
    * Return a compact SAN representation for the move.
    */
  virtual QString SAN(boost::shared_ptr<AbstractPosition> ref) const = 0;

  /**
    * Return a decorated representation for the move.
    */
  virtual DecoratedMove toDecoratedMove(boost::shared_ptr<AbstractPosition> ref) const = 0;

  /**
    * Return the move in coordinate notation.
    */
  virtual QString toString(boost::shared_ptr<AbstractPosition> ref) const = 0;

  /**
    * Convert the move to a normal user move. Used to
    * perform move highlighting.
    */
  virtual NormalUserMove toUserMove() const = 0;

  virtual bool equals(Ptr other) const = 0;

};

/**
  * @brief A superclass for all the position classes.
  *
  * A general interface for positions. It is used from within
  * the graphical framework to validate external input wrt
  * the played variant.
  */
class AbstractPosition {
public:
  typedef boost::shared_ptr<AbstractPosition> Ptr;
  typedef std::map<AbstractPiece::Ptr, int, AbstractPieceComparer> AbstractPool;
  typedef boost::shared_ptr<AbstractPool> PoolPtr;
  virtual ~AbstractPosition() { }

  /**
    * Return board size in logical units. This could be
    * static if C++ permitted virtual static functions.
    */
  virtual Point size() const = 0;

  /**
    * Returns the text to be used for the border surrounding this position in a board
    */
  virtual QStringList borderCoords() const = 0;

  /**
    * Create the starting piece setup.
    */
  virtual void setup() = 0;

  /**
    * Retrieve the pieces in the piece pool.
    */
  virtual PoolPtr pool() const = 0;

  /**
    * Add a piece to the pool n times.
    */
  virtual void addToPool(AbstractPiece::Ptr piece, int n) = 0;

  /**
    * Remove a piece from the pool n times.
    */
  virtual void removeFromPool(AbstractPiece::Ptr piece, int n) = 0;

  /**
    * Copies the pool of a position.
    */
  virtual void copyPoolFrom(AbstractPosition::Ptr pos) = 0;

  /**
    * Sets the pool from a PoolPtr.
    */
  virtual void setPool(PoolPtr pool) = 0;

  /**
    * Retrieve the piece on square @a p.
    * Return a null pointer if that square is empty.
    */
  virtual AbstractPiece::Ptr get(const Point& p) const = 0;

  /**
    * Set a piece on the board.
    */
  virtual void set(const Point& p, AbstractPiece::Ptr piece) = 0;

  /**
    * Return an id corresponding to the player
    * who is in turn.
    */
  virtual int turn() const = 0;
  virtual void setTurn(int) = 0;

  virtual int previousTurn() const = 0;

  /**
    * Switch to the next player.
    */
  virtual void switchTurn() = 0;

  /**
    * Check move legality. Set move fields as needed.
    * Return whether the move is legal.
    * This function should return immediately if @a m
    * has already been tested.
    */
  virtual bool testMove(AbstractMove::Ptr m) const = 0;

  /**
    * Execute move @a m. Assume that m is legal and tested.
    */
  virtual void move(AbstractMove::Ptr m) = 0;

  /**
    * Create a deep copy of the position.
    */
  virtual AbstractPosition::Ptr clone() const = 0;

  /**
    * Tests if two positions are equal.
    */
  virtual bool equal(AbstractPosition::Ptr p) const = 0;

  /**
    * Return a move from an algebraic notation, or a null pointer.
    */
  virtual AbstractMove::Ptr getMove(const class AlgebraicNotation&) const = 0;

  /**
    * Return a move from an algebraic notation, or a null pointer.
    */
  virtual AbstractMove::Ptr getMove(const QString&) const = 0;

  /**
    * Return a string representing the current state of the position.
    */
  virtual QString state() const = 0;

  /**
    * Return a FEN representation for the position, assuming
    * @a halfmove as halfmove clock and @a fullmove as full move
    * number.
    */
  virtual QString fen(int halfmove, int fullmove) const = 0;

  /**
    * A piece somehow representing or related to the move
    * which has to be drawn by the interface.
    * Examples:
    * * for one click moves, the piece that would
    *    appear on the square.
    * * for promotions in chesslike variants, the promoted
    *   piece.
    * @note Return a null pointer if the move has no valid
    *       piece hint defined.
    */
  virtual AbstractPiece::Ptr moveHint(AbstractMove::Ptr) const = 0;

  /**
    * Variant introspection
    */
  virtual QString variant() const = 0;

  /**
    * For debugging
    */
  virtual void dump() const = 0;
};

class AnimationGroup;

/**
  * @brief An abstract superclass for all animator classes.
  *
  * An animator is a class which is given a difference between
  * a graphical and a logical position, and schedules an animation
  * which graphically does an update.
  * If the difference is due to a move, the animator tries to
  * create a smooth animation which could possibly work for many
  * chess-like variants, and then fills in the gaps with a direct
  * update.
  */
class AbstractAnimator {
public:
  typedef boost::shared_ptr<AbstractAnimator> Ptr;
  typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
  virtual ~AbstractAnimator() { }

  /**
    * Sync the graphical position with the given position.
    */
  virtual AnimationPtr warp(AbstractPosition::Ptr) = 0;

  /**
    * Animate forward syncing to the given position.
    */
  virtual AnimationPtr forward(AbstractPosition::Ptr, AbstractMove::Ptr) = 0;

  /**
    * Animate back syncing to the given position.
    */
  virtual AnimationPtr back(AbstractPosition::Ptr, AbstractMove::Ptr) = 0;
};

class PointConverter;
class GraphicalPosition;
namespace PixmapLoader{ class Info; }

class VariantInfo {
public:
  virtual ~VariantInfo() { }
  virtual AbstractPosition::Ptr createPosition() = 0;
  virtual AbstractPosition::Ptr createCustomPosition(const OptList& l) = 0;
  virtual AbstractPosition::Ptr createPositionFromFEN(const QString& fen) = 0;
  virtual AbstractPosition::Ptr createChessboard(int turn, bool, bool, bool, bool, const Point&) = 0;
  virtual AbstractPiece::Ptr createPiece(int color, int type) = 0;
  virtual void forallPieces(class PieceFunction&) = 0;
  virtual int moveListLayout() const = 0;
  virtual AbstractAnimator::Ptr createAnimator(PointConverter* converter,
                                           GraphicalPosition* position) = 0;
  virtual AbstractMove::Ptr createNormalMove(const NormalUserMove&) = 0;
  virtual AbstractMove::Ptr createDropMove(const DropUserMove&) = 0;
  virtual AbstractMove::Ptr getVerboseMove(int turn, const class VerboseNotation&) const = 0;
  virtual int type(const QString& x) = 0;
  virtual QString typeSymbol(int type) = 0;
  virtual bool simpleMoves() = 0;
  virtual QString name() const = 0;
  virtual QString themeProxy() const = 0;
  virtual OptList positionOptions() const = 0;
};

#endif // LOWLEVEL_H
