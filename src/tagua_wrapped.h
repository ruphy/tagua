/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HIGHLEVEL_H
#define HIGHLEVEL_H

#include <memory>
#include <iostream>
#include "common.h"
#include "icsapi.h"
#include "tagua.h"
#include "movefactory.h"
#include "piecefactory.h"
#include "moveserializer.h"
#include "nopool.h"
#include "algebraicnotation.h"

template <typename V> class WrappedPiece;
template <typename V> class WrappedMove;
template <typename V> class WrappedPosition;

/**
  * @brief Wrap an high level position definition
  *        in a low level AbstractPosition implementation.
  *
  * Use this class to wrap an high level position definition
  * in an AbstractPosition implementation suitable for communicating
  * with the tagua infrastructure.
  * Variant should be a structure with inner types Position, Move and Piece.
  *
  * @section Requirements on Position
  *
  * Position should define the following functions:
  * - static Point size();
  * - Piece* operator[](const Point& p) const;
  * - Piece::Color turn() const;
  * - void switchTurn();
  * - bool testMove(Move& m) const;
  * - void move(const Move& m);
  * - void setup();
  *
  * @section Requirements on Piece
  *
  * Piece should have inner types Color and Type, each
  * convertible to int via static_cast.
  * Furthermore, it should implement the following
  * functions:
  * - Piece::Type type() const;
  * - Piece::Color color() const;
  * - QString name() const;
  * - static QString typeSymbol(Piece::Type type);
  *
  * @section Move serialization
  *
  * To customize move serialization, one should specialize
  * the template MoveSerializer<Pos>.
  *
  */

#ifdef Q_CC_GNU
  #define __FUNC__ __PRETTY_FUNCTION__
#else
  #define __FUNC__ __FUNCTION__
#endif

#define MISMATCH(x,y) (std::cout << " --> Error in "<<__FUNC__<<", MISMATCH!" << std::endl \
                       << "     got type   " << prettyTypeName(typeid(x).name()) << std::endl \
                       << "     instead of " << prettyTypeName(typeid(y).name()) << std::endl \
                       << "     this is    " << prettyTypeName(typeid(*this).name()) << std::endl)

#define TYPECHECK(x,y) if (typeid(x) != typeid(y)) MISMATCH(x,y); else { }

/**
  * Helper metafunction to create a null ICSAPI object
  * if the variant does not support ICS.
  */
template <typename Variant, bool hasICS>
struct ReturnICSAPI { };

template <typename Variant>
struct ReturnICSAPI<Variant, true> {
  static ICSAPI* apply() {
    return new WrappedICSAPI<Variant>();
  }
};

template <typename Variant>
struct ReturnICSAPI<Variant, false> {
  static ICSAPI* apply() { return 0; }
};


template <typename Variant>
class WrappedPiece : public AbstractPiece {
  typedef typename Variant::Piece Piece;

  Piece m_piece;
public:
  const Piece& inner() const { return m_piece; }

  WrappedPiece(const Piece& piece)
  : m_piece(piece) { }

  virtual bool equals(const PiecePtr& _other) const {
    if (!_other) return false;
    WrappedPiece<Variant>* other = dynamic_cast<WrappedPiece<Variant>*>(_other.get());

    if (other)
      return m_piece == other->inner();
    else {
      MISMATCH(*_other.get(),WrappedPiece<Variant>);
      return false;
    }
  }

  virtual QString name() const {
    return m_piece.name();
  }

  virtual QString typeSymbol() const {
    return Piece::typeSymbol(m_piece.type());
  }

  virtual PiecePtr clone() const {
    return PiecePtr(new WrappedPiece<Variant>(m_piece));
  }
};


template <typename Variant>
class WrappedMove : public AbstractMove {
  typedef typename Variant::Move Move;
  typedef typename Variant::Position Position;

  Move m_move;
public:
  const Move& inner() const { return m_move; }

  WrappedMove(const Move& move)
  : m_move(move) { }

  virtual QString SAN(const PositionPtr& _pos) const {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());

    if (pos) {
      MoveSerializer<Position> serializer(m_move, pos->inner());
      return serializer.SAN();
    }
    else {
      MISMATCH(*_pos.get(),WrappedPosition<Variant>);
      return "$@%";
    }
  }

  virtual DecoratedMove toDecoratedMove(const PositionPtr& _pos) const {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());

    if (pos) {
      MoveSerializer<Position> serializer(m_move, pos->inner());
      return serializer.toDecoratedMove();
    }
    else {
      MISMATCH(*_pos.get(),WrappedPosition<Variant>);
      return DecoratedMove();
    }
  }

  virtual QString toString(const PositionPtr& _pos) const {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());

    if (pos)
      return m_move.toString(pos->inner().size().x, pos->inner().size().y);
    else {
      MISMATCH(*_pos.get(),WrappedPosition<Variant>);
      return "$@%";
    }
  }

  virtual NormalUserMove toUserMove() const {
    return MoveFactory<Variant>::toNormal(m_move);
  }

  virtual bool equals(const MovePtr& _other) const {
    WrappedMove<Variant>* other = dynamic_cast<WrappedMove<Variant>*>(_other.get());

    if (other)
      return m_move == other->inner();
    else {
      MISMATCH(*_other.get(),WrappedMove<Variant>);
      return false;
    }
  }
};

template <typename Variant, typename Pool>
class WrappedPoolBase : public AbstractPool {
  typedef typename Variant::Piece Piece;

  Pool m_pool;
public:
  WrappedPoolBase(Pool pool)
  : m_pool(pool) { }

  virtual int size() {
    return m_pool.size();
  }

  virtual int insert(int pref_index, const PiecePtr& _piece) {
    if (!_piece) {
      return m_pool.insert(pref_index, Piece());
    }
    else {
      WrappedPiece<Variant>* piece = dynamic_cast<WrappedPiece<Variant>*>(_piece.get());

      if (piece)
        return m_pool.insert(pref_index, Piece(piece->inner()) );
      else {
        MISMATCH(*_piece.get(),WrappedPiece<Variant>);
        return -1;
      }
    }
  }

  virtual PiecePtr get(int index) {
    Piece piece = m_pool.get(index);
    if (piece)
      return PiecePtr(new WrappedPiece<Variant>(piece));
    else
      return PiecePtr();
  }

  virtual PiecePtr take(int index) {
    Piece piece = m_pool.take(index);
    if (piece)
      return PiecePtr(new WrappedPiece<Variant>(piece));
    else
      return PiecePtr();
  }
};

// simple special case for NoPool
template <typename Variant>
class WrappedPoolBase<Variant, NoPool> : public AbstractPool {
public:
  WrappedPoolBase(NoPool) { }
};

template <typename Variant>
class WrappedPool : public WrappedPoolBase<Variant, typename Variant::Pool> {
public:
  WrappedPool(typename Variant::Pool pool)
  : WrappedPoolBase<Variant, typename Variant::Pool>(pool) { }
};

/**
  * Metafunction that returns a null pointer when
  * its template argument is NoPool.
  */
template <typename Variant, typename Pool>
struct ReturnPool {
  static PoolPtr apply(typename Variant::Position& position, int player) {
    return PoolPtr(
      new WrappedPool<Variant>(position.pool(player)));
  }
};

template <typename Variant>
struct ReturnPool<Variant, NoPool> {
  static PoolPtr apply(typename Variant::Position&, int) {
    return PoolPtr();
  }
};

/**
  * Metafunction to assign pools of a position to another.
  */
template <typename Variant, typename Pool>
struct AssignPool {
  static void apply(typename Variant::Position& pos1, const typename Variant::Position& pos2) {
    pos1.setRawPool(pos2.rawPool());
  }
};

template <typename Variant>
struct AssignPool<Variant, NoPool> {
  static void apply(typename Variant::Position&, const typename Variant::Position&) { }
};

template <typename Variant>
class WrappedPosition : public AbstractPosition {
  typedef typename Variant::Position Position;
  typedef typename Variant::Move Move;
  typedef typename Variant::Piece Piece;
  typedef typename Variant::Pool Pool;

  Position m_pos;
public:
  const Position& inner() const { return m_pos; }
  Position& inner() { return m_pos; }

  WrappedPosition(const Position& pos)
  : m_pos(pos) { }

  virtual Point size() const {
    return inner().size();
  }

  virtual QStringList borderCoords() const {
    return inner().borderCoords();
  }

  virtual void setup() {
    m_pos.setup();
  }

  virtual PiecePtr get(const Point& p) const {
    Piece piece = m_pos.get(p);
    if (piece)
      return PiecePtr(new WrappedPiece<Variant>(piece));
    else
      return PiecePtr();
  }

  virtual void set(const Point& p, const PiecePtr& _piece) {
    if (!_piece) {
      m_pos.set(p, Piece());
    }
    else {
      WrappedPiece<Variant>* piece = dynamic_cast<WrappedPiece<Variant>*>(_piece.get());

      if (piece)
        m_pos.set(p, piece->inner() );
      else
        MISMATCH(*_piece.get(),WrappedPiece<Variant>);
    }
  }

  virtual PoolPtr pool(int player) {
    return ReturnPool<Variant, Pool>::apply(m_pos, player);
  }
  
  virtual void copyPoolFrom(const PositionPtr& _pos) {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
    if (pos) {
      AssignPool<Variant, Pool>::apply(m_pos, pos->inner());
    }
    else {
      MISMATCH(*_pos.get(), WrappedPosition<Variant>);
    }
  }

  virtual InteractionType movable(const TurnTest& test, const Point& p) const {
    return m_pos.movable(test, p);
  }

  virtual InteractionType droppable(const TurnTest& test, int p) const {
    return m_pos.droppable(test, p);
  }

  virtual int turn() const {
    return static_cast<int>(m_pos.turn());
  }

  virtual void setTurn(int turn) {
    m_pos.setTurn(static_cast<typename Piece::Color>(turn));
  }

  virtual int previousTurn() const {
    return static_cast<int>(m_pos.previousTurn());
  }

  virtual void switchTurn() {
    m_pos.switchTurn();
  }

  virtual bool testMove(const MovePtr& _move) const {
    WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());

    if (move)
      return m_pos.testMove( const_cast<Move&>(move->inner()) );
    else {
      MISMATCH(*_move.get(),WrappedMove<Variant>);
      return false;
    }
  }

  virtual void move(const MovePtr& _move) {
    WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());

    if (move)
      m_pos.move(move->inner());
    else
      MISMATCH(*_move.get(),WrappedMove<Variant>);
  }

  virtual PositionPtr clone() const {
    return PositionPtr(new WrappedPosition<Variant>(m_pos));
  }

  virtual void copyFrom(const PositionPtr& _p) {
    WrappedPosition* p = dynamic_cast<WrappedPosition*>(_p.get());

    if (p)
      m_pos = p->inner();
    else
      MISMATCH(*_p.get(),WrappedPosition);
  }

  virtual bool equals(const PositionPtr& _other) const {
    WrappedPosition<Variant>* other = dynamic_cast<WrappedPosition<Variant>*>(_other.get());

    if(other)
      return m_pos == other->inner();
    else {
      MISMATCH(*_other.get(),WrappedPosition<Variant>);
      return false;
    }
  }

  virtual MovePtr getMove(const AlgebraicNotation& san) const {
    bool ok;
    Move res = m_pos.getMove(san, ok);
    if (ok)
      return MovePtr(new WrappedMove<Variant>(res));
    else
      return MovePtr();
  }

  virtual MovePtr getMove(const QString& san) const {
    AlgebraicNotation move(san, size().y); //FIXME
    if(!move.valid())
      return MovePtr();
    return getMove(move);
  }

  virtual QString state() const {
    return ""; // TODO
  }

  virtual QString fen(int halfmove, int fullmove) const {
    return m_pos.fen(halfmove, fullmove);
  }

  virtual PiecePtr moveHint(const MovePtr& _move) const {
    WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());

    if (move) {
      if(boost::shared_ptr<Piece> hint = m_pos.moveHint(move->inner()))
        return PiecePtr(
          new WrappedPiece<Variant>(Piece(*hint)));
    }
    else {
      MISMATCH(*_move.get(),WrappedMove<Variant>);
    }

    return PiecePtr();
  }

  virtual QString variant() const {
    return Variant::m_name;
  }

  virtual void dump() const {
    m_pos.dump();
  }
};


template <typename Variant>
class WrappedAnimator : public AbstractAnimator {
  typedef typename Variant::Position Position;
  typedef typename Variant::Animator Animator;
  typedef typename Variant::Move Move;

  Animator m_animator;
public:
  const Animator& inner() const { return m_animator; }

  WrappedAnimator(const Animator& animator)
  : m_animator(animator) { }

  virtual AnimationPtr warp(const PositionPtr& _pos) {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
    if (pos)
      return m_animator.warp(pos->inner());
    else {
      MISMATCH(*_pos.get(), WrappedPosition<Variant>);
      return AnimationPtr();
    }
  }

  virtual AnimationPtr forward(const PositionPtr& _pos, const MovePtr& _move) {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
    WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());

    if (move && pos)
      return m_animator.forward(pos->inner(), move->inner());
    else {
      if (!move)
        MISMATCH(*_move.get(), WrappedMove<Variant>);
      if (!pos)
        MISMATCH(*_pos.get(), WrappedPosition<Variant>);
      return AnimationPtr();
    }
  }

  virtual AnimationPtr back(const PositionPtr& _pos, const MovePtr& _move) {
    WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
    WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());

    if (move && pos)
      return m_animator.back(pos->inner(), move->inner());
    else {
      if (!move)
        MISMATCH(*_move.get(), WrappedMove<Variant>);
      if (!pos)
        MISMATCH(*_pos.get(), WrappedPosition<Variant>);
      return AnimationPtr();
    }
  }
};

#include "graphicalapi_unwrapped.h"

template <typename Variant>
class WrappedVariantInfo : public VariantInfo {
public:
  typedef typename Variant::Animator Animator;
  typedef typename Variant::Position Position;
  typedef typename Variant::Piece Piece;
  typedef typename Variant::Move Move;
  typedef typename Variant::Pool Pool;

  virtual PositionPtr createPosition() {
    return PositionPtr(
      new WrappedPosition<Variant>(Position()));
  }

  virtual PositionPtr createCustomPosition(const OptList& l) {
    return PositionPtr(
      new WrappedPosition<Variant>(Position(l)));
  }

  virtual PositionPtr createPositionFromFEN(const QString& fen) {
    std::auto_ptr<WrappedPosition<Variant> > res(
      new WrappedPosition<Variant>(Position()));
    bool ok;
    res->inner().fromFEN(fen, ok);
    if (ok) {
      return PositionPtr(res.release());
    }
    else return PositionPtr();
  }
  virtual void forallPieces(class PieceFunction& f) {
    Variant::forallPieces(f);
  }
  virtual int moveListLayout() const {
    return Variant::moveListLayout();
  }
  virtual AnimatorPtr createAnimator(GraphicalAPI* graphical_api) {
    return AnimatorPtr(
      new WrappedAnimator<Variant>(
              Animator(typename UnwrappedGraphicalAPI<Variant>::Ptr(
                    new UnwrappedGraphicalAPI<Variant>(graphical_api)))));
  }
  virtual MovePtr createNormalMove(const NormalUserMove& move) {
    return MovePtr(new WrappedMove<Variant>(
      MoveFactory<Variant>::createNormalMove(move)));
  }
  virtual MovePtr createDropMove(const DropUserMove& move) {
    return MovePtr(new WrappedMove<Variant>(
      MoveFactory<Variant>::createDropMove(move)));
  }

  virtual MovePtr getVerboseMove(int turn, const VerboseNotation& m) const {
    Move res = Position::getVerboseMove(static_cast<typename Piece::Color>(turn), m);
    return MovePtr(new WrappedMove<Variant>(res));
  }

  virtual int type(const QString& str) {
    return Piece::getType(str);
  }
  virtual QString typeSymbol(int type) {
    return Piece::typeSymbol(static_cast<typename Piece::Type>(type));
  }
  virtual bool simpleMoves() {
    return Variant::m_simple_moves;
  }
  virtual QString name() const {
    return Variant::m_name;
  }
  virtual QString themeProxy() const {
    return Variant::m_theme_proxy;
  }
  virtual OptList positionOptions() const {
    return Variant::positionOptions();
  }
  virtual ICSAPIPtr icsAPI() const {
    return ICSAPIPtr(ReturnICSAPI<Variant, Variant::hasICS>::apply());
  }
};

#endif // HIGHLEVEL_H
