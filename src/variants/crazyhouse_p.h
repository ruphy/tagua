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
#include "xchess/pool.h"

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

class CrazyhouseMove : public ChessMove {
  CrazyhousePiece m_drop;
  int m_pool;
  int m_pool_index;
public:
  CrazyhouseMove(const ChessMove& move);
  CrazyhouseMove(const Point& from, const Point& to, PieceType promotionType = INVALID_TYPE);
  CrazyhouseMove(const CrazyhousePiece& p, const Point& to);
  CrazyhouseMove(int pool, int m_pool_index, const Point& to);
  CrazyhouseMove(const CrazyhouseMove&);

  QString toString(int ysize) const {
    if(m_drop.valid())
      return CrazyhousePiece::typeSymbol(m_drop.type()) + "@" + to.toString(ysize);
    return ChessMove::toString(ysize);
  }
  static CrazyhouseMove createDropMove(int pool, int m_pool_index, const Point& to) {
    return CrazyhouseMove(pool, m_pool_index, to);
  }
  
  CrazyhousePiece drop() const { return m_drop; }
  void setDrop(const CrazyhousePiece& piece) { m_drop = piece; }
  int pool() const { return m_pool; }
  int poolIndex() const { return m_pool_index; }
};

class CrazyhousePosition : public Position<CrazyhouseMove, CrazyhousePiece, Grid<CrazyhousePiece> > {
public:
  typedef CrazyhouseMove Move;
  typedef CrazyhousePiece Piece;
  typedef Position<Move, Piece, Grid<Piece> > Base;
  typedef PoolReference<CrazyhousePosition> PoolReference;
  typedef PoolConstReference<CrazyhousePosition> PoolConstReference;
  typedef PoolReference::Pool Pool;
  typedef PoolReference::PlayerPool PlayerPool;

  CrazyhousePosition();
  CrazyhousePosition(const OptList& l);
  CrazyhousePosition(const CrazyhousePosition&);
  CrazyhousePosition(const ChessPosition&);
  CrazyhousePosition(CrazyhousePiece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  virtual CrazyhousePosition* clone() const;


  Pool m_pool;
public:
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
      retv.setDrop(CrazyhousePiece(turn, static_cast<ChessPiece::Type>(m.type)));
    else
      retv.setDrop(CrazyhousePiece(INVALID_COLOR, INVALID_TYPE));
    return retv;
  }

  void dump() const;
  
  PoolReference pool(int index);
  PoolConstReference pool(int index) const;
    
  PlayerPool& rawPool(Piece::Color color);
  const PlayerPool& rawPool(Piece::Color color) const;
  
  void setRawPool(const Pool& p) { m_pool = p; }
  Pool& rawPool() { return m_pool; }
  const Pool& rawPool() const { return m_pool; }
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
    const CrazyhousePosition::PlayerPool& pp = m_pos.rawPool()[m_pos.turn()];
    for (CrazyhousePosition::PlayerPool::const_iterator it = pp.begin();
          it != pp.end(); ++it) {
      for (Point to = m_pos.first();
          to <= m_pos.last();
          to = m_pos.next(to)) {
        CrazyhouseMove move(CrazyhousePiece(m_pos.turn(),it->first), to);
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
    if (m_move.drop()) {

      return QString("%1@%2")
              .arg(CrazyhousePiece::typeSymbol(m_move.drop().type()))
              .arg(m_move.to.toString(m_ref.size().y)) + checkSuffix();
    }
    else
      return Base::SAN();
  }
};

#endif // CRAZYHOUSE_P_H
