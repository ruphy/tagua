/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CRAZYHOUSE_P_H
#define CRAZYHOUSE_P_H

#include <map>
#include "chess.h"

class CrazyhousePiece : public ChessPiece {
  bool m_promoted;

public:
  CrazyhousePiece(ChessPiece::Color = INVALID_COLOR, ChessPiece::Type = INVALID_TYPE,
                                                              bool promoted = false);
  CrazyhousePiece(const CrazyhousePiece&);
  CrazyhousePiece(const ChessPiece&);

  inline bool promoted() const { return m_promoted; }
  inline void setPromoted(bool value) { m_promoted = value; }

  bool operator<(const CrazyhousePiece& p) const {
    return m_promoted != p.m_promoted ?
            m_promoted < p.m_promoted :
            this->ChessPiece::operator<(p);
  }
};

// here we use inheritance, but the best approach would be
// composition + delegation, since inheriting we get an unwanted
// automatic conversion CrazyhouseMove -> ChessMove
/*      UPDATE: is this still true? Maurizio               */
class CrazyhouseMove : public ChessMove {
public:
  CrazyhousePiece m_drop;

  CrazyhouseMove(const ChessMove& move);
  CrazyhouseMove(const Point& from, const Point& to, PieceType promotionType = INVALID_TYPE);
  CrazyhouseMove(CrazyhousePiece piece, const Point& to);
  CrazyhouseMove(const CrazyhouseMove&);

  QString toString(int ysize) const {
    if(m_drop.valid())
      return CrazyhousePiece::typeSymbol(m_drop.type()) + "@" + to.toString(ysize);
    return ChessMove::toString(ysize);
  }
  static CrazyhouseMove createDropMove(const CrazyhousePiece& p, const Point& to) {
    return CrazyhouseMove(p, to);
  }
};

class CrazyhousePosition : public Position<CrazyhouseMove, CrazyhousePiece, Grid<CrazyhousePiece> > {
public:
  typedef CrazyhouseMove Move;
  typedef CrazyhousePiece Piece;
  typedef Position<Move, Piece, Grid<Piece> > Base;
  typedef std::map<CrazyhousePiece, int> Pool;

  CrazyhousePosition();
  CrazyhousePosition(const OptList& l);
  CrazyhousePosition(const CrazyhousePosition&);
  CrazyhousePosition(const ChessPosition&);
  CrazyhousePosition(CrazyhousePiece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  virtual CrazyhousePosition* clone() const;

public:
  virtual void addToPool(const Piece& p, int n) { m_pool[p] += n; }
  virtual void removeFromPool(const Piece& p, int n) {
    if((m_pool[p] -= n) <= 0)
      m_pool.erase(p);
  }

  virtual CrazyhousePiece::Color moveTurn(const Move&) const;
  virtual bool pseudolegal(Move&) const;
  virtual void move(const Move&);
  virtual void executeCaptureOn(const Point& point);
  virtual boost::shared_ptr<AbstractGenerator<Move> > createLegalGenerator() const;

  virtual Move getMove(const AlgebraicNotation& san, bool& ok) const;

  virtual bool operator==(const CrazyhousePosition& other) const;

  static Move getVerboseMove(Color turn, const VerboseNotation& m) {
    Move retv = ChessPosition::getVerboseMove(turn, m);
    if(retv.from == Point::invalid())
      retv.m_drop = CrazyhousePiece(turn, static_cast<ChessPiece::Type>(m.type) );
    else
      retv.m_drop = CrazyhousePiece(INVALID_COLOR, INVALID_TYPE);
    return retv;
  }
};

template <typename MoveTest>
class MoveGenerator<CrazyhousePosition, MoveTest>
    : public Generator<CrazyhousePosition, MoveTest> {
    typedef Generator<CrazyhousePosition, MoveTest> Base;
  using Base::m_pos;
  using Base::m_test;
  using Base::m_moves;
public:
  MoveGenerator(const CrazyhousePosition& pos)
  : Base(pos) { }

  std::vector<CrazyhouseMove>& generate() {
    generateDrops();
    return Base::generate();
  }

private:
  void generateDrops() {
    for (std::map<CrazyhousePiece, int>::const_iterator it = m_pos.pool().begin();
          it != m_pos.pool().end(); ++it) {
      if(m_pos.turn() == it->first.color())
      for (Point to = m_pos.first();
          to <= m_pos.last();
          to = m_pos.next(to)) {
        CrazyhouseMove move(it->first, to);
        if (m_test(move)) m_moves.push_back(move);
      }
    }
  }
};

template <>
class MoveSerializer<CrazyhousePosition> : public MoveSerializerBase<CrazyhousePosition> {
  typedef CrazyhousePosition Position;
  typedef CrazyhouseMove Move;
  typedef CrazyhousePiece Piece;
  typedef MoveSerializerBase<Position> Base;
public:
  MoveSerializer(const Move& move, const Position& ref)
  : MoveSerializerBase<Position>(move, ref) { }

  QString SAN() const {
    if (m_move.m_drop.valid()) {

      return QString("%1@%2")
              .arg(CrazyhousePiece::typeSymbol(m_move.m_drop.type()))
              .arg(m_move.to.toString(m_ref.size().y)) + checkSuffix();
    }
    else
      return Base::SAN();
  }
};

class CrazyhouseAnimator : public ChessAnimator {
public:
  CrazyhouseAnimator(PointConverter* converter, GraphicalPosition* position);

  virtual AnimationPtr warp(AbstractPosition::Ptr);
  virtual AnimationPtr forward(AbstractPosition::Ptr, const CrazyhouseMove& move);
  virtual AnimationPtr back(AbstractPosition::Ptr, const CrazyhouseMove& move);
};

#endif // CRAZYHOUSE_P_H