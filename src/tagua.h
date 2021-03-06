/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LOWLEVEL_H
#define LOWLEVEL_H

#include <map>
#include <boost/shared_ptr.hpp>
#include <QString>
#include <QStringList>
#include "point.h"
#include "usermove.h"
#include "index.h"
#include "option.h"
#include "decoratedmove.h"
#include "interactiontype.h"
#include "turnpolicy.h"
#include "fwd.h"

class GraphicalAPI;
class ICSAPI;
class ActionCollection;

/**
  * @file tagua.h
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
  * The file @ref tagua_wrapped.h tries to address this problem doing part
  * of the compiler's work. One exception is that type checking code
  * is inserted in a method wrapper, instead of the calling place, so
  * even calling a method of a noncasted instance would cause some
  * upcast / downcast overhead; this is negligible, though, because
  * that scenario is rather unlikely, since wrapped classes are always
  * kept in variables of abstract types.
  */


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
  virtual bool equals(const PiecePtr& other) const = 0;

  /**
    * Return a unique key for the piece.
    * Used to store pixmaps.
    */
  virtual QString name() const = 0;

  /**
    * Create a deep copy of the piece.
    */
  virtual PiecePtr clone() const = 0;
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
    * Serialize the move.
    * \param rep The type of representation to use.
    * \param ref A reference position. Its meaning is defined by the serializer.
    */
  virtual QString toString(const QString& rep, const PositionPtr& ref) const = 0;

  /**
    * Convert the move to a normal user move. Used to
    * perform move highlighting.
    */
  virtual NormalUserMove toUserMove() const = 0;

  /**
    * Checks if the two moves are equal.
    */
  virtual bool equals(const MovePtr& other) const = 0;

};

/**
  * @brief Superclass for pools
  *
  * A general interface for pools.
  */
class AbstractPool {
public:
  typedef boost::shared_ptr<AbstractPool> Ptr;
  virtual ~AbstractPool() {}

  /**
    * \return the number of items in the pool
    */
  virtual int size() = 0;

  /**
    * Inserts a piece in the pool, preferably at the position \a pref_index.
    * But the pool can be unpredictable and the piece can be placed at an arbitrary position.
    * \return the position at which the item was placed.
    */
  virtual int insert(int pref_index, const PiecePtr& piece) = 0;

  /**
    * Gets the piece at the position \a index in the pool.
    */
  virtual PiecePtr get(int index) = 0;

  /**
    * Removes the piece at the position \a index in the pool.
    * \return the removed piece.
    */
  virtual PiecePtr take(int index) = 0;
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
    * Retrieve the piece on square @a p.
    * Return a null pointer if that square is empty.
    */
  virtual PiecePtr get(const Point& p) const = 0;

  /**
    * Set a piece on the board.
    */
  virtual void set(const Point& p, const PiecePtr& piece) = 0;

  /**
    * \return an interface to modify the pool of the board relative to \a player
    */
  virtual PoolPtr pool(int player) = 0;
  
  /**
    * Set a position pool, copying it from a given position.
    */
  virtual void copyPoolFrom(const PositionPtr& pos) = 0;

  /**
    * \return 1 if the piece can be moved, -1 if could be moved in the future (premove), or else 0.
    */
  virtual InteractionType movable(const TurnPolicy::Collection& test, const Point& p) const = 0;

  /**
    * \return 1 if this pool can be dropped, -1 if could be dropped in the future (premove), or else 0.
    */
  virtual InteractionType droppable(const TurnPolicy::Collection& test, int) const = 0;

  /**
    * \return an id corresponding to the player
    * who is in turn.
    */
  virtual int turn() const = 0;

  /**
    * Sets the player on move
    */
  virtual void setTurn(int) = 0;

  /**
    * \return the player that just moved
    */
  virtual int previousTurn() const = 0;

  /**
    * Switch to the next player.
    */
  virtual void switchTurn() = 0;

  /**
    * Check move legality. Set move fields as needed.
    * Return whether the move is legal.
    * This function should return immediately if \a m
    * has already been tested.
    */
  virtual bool testMove(const MovePtr& m) const = 0;

  /**
    * Execute move \a m. Assume that \a m is legal and tested.
    */
  virtual void move(const MovePtr& m) = 0;

  /**
    * Create a deep copy of the position.
    */
  virtual PositionPtr clone() const = 0;

  /**
    * Make the position equal to the given one.
    */
  virtual void copyFrom(const PositionPtr&) = 0;

  /**
    * Tests if two positions are equal.
    */
  virtual bool equals(const PositionPtr& p) const = 0;

  /**
    * Return a move from an algebraic notation, or a null pointer.
    */
  virtual MovePtr getMove(const QString&) const = 0;

  /**
    * Return a string representing the current state of the position.
    */
  virtual QString state() const = 0;

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
  virtual PiecePtr moveHint(const MovePtr&) const = 0;

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
  virtual ~AbstractAnimator() { }

  /**
    * Sync the graphical position with the given position.
    */
  virtual AnimationPtr warp(const PositionPtr&) = 0;

  /**
    * Animate forward syncing to the given position.
    */
  virtual AnimationPtr forward(const PositionPtr&, const MovePtr&) = 0;

  /**
    * Animate back syncing to the given position.
    */
  virtual AnimationPtr back(const PositionPtr&, const MovePtr&) = 0;
};


class VariantInfo {
public:
  virtual ~VariantInfo() { }
  virtual PositionPtr createPosition() = 0;
  virtual PositionPtr createCustomPosition(const OptList& l) = 0;
  virtual void forallPieces(class PieceFunction&) = 0;
  virtual int moveListLayout() const = 0;
  virtual AnimatorPtr createAnimator(GraphicalAPI* graphical_api) = 0;
  virtual MovePtr createNormalMove(const NormalUserMove&) = 0;
  virtual MovePtr createDropMove(const DropUserMove&) = 0;
  virtual MovePtr getVerboseMove(int turn, const class VerboseNotation&) const = 0;

  /**
    * \return if moves are done by just clicking
    */
  virtual bool simpleMoves() = 0;

  /**
    * \return the name of the variant
    */
  virtual QString name() const = 0;

  /**
    * \return the name of the theme proxy variant, ie the variant whose theme can be used
    * for the current one (for instance crazyhouse can use chess themes).
    */
  virtual QString themeProxy() const = 0;

  /**
    * \return the (subvariant) options that can be specified for position creation, such as board size, etc
    */
  virtual OptList positionOptions() const = 0;
  
  /**
    * \return The ICS API for this variant, or a null pointer, if the variant does not support
    *         ICS (in that case a Dummy variant will be used).
    */
  virtual ICSAPIPtr icsAPI() const = 0;
  
  /**
    * Setup a list of variant specific actions to be displayed on a game toolbar
    * and menu.
    */
  virtual ActionCollection* actions() = 0;
  
  /**
    * Add variant specific information (i.e. promotionType) to a move.
    */
  virtual void setupMove(NormalUserMove& m) const = 0;
};


class VariantFactory {
public:
  virtual ~VariantFactory() { }
  virtual VariantInfo* createVariant() const = 0;
  virtual QString name() const = 0;
  virtual QString themeProxy() const = 0;
  virtual bool hidden() const = 0;
};


#endif // LOWLEVEL_H
