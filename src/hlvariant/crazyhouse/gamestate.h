/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__GAMESTATE_H
#define HLVARIANT__CRAZYHOUSE__GAMESTATE_H

#include "../chess/gamestate.h"
#include "../pool.h"
#include "../poolcollection.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename _Board, typename _Move>
class GameState : public HLVariant::Chess::GameState<_Board, _Move> {
  typedef HLVariant::Chess::GameState<_Board, _Move> Base;
public:
  typedef typename Base::Board Board;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
  typedef Pool<Piece> Pool;
  typedef PoolCollection<Pool> Pools;
private:
  using Base::m_board;
  Pools m_pools;
public:
  GameState();
  GameState(typename Piece::Color, bool, bool, bool, bool, const Point&);

  virtual const Pools& pools() const;
  virtual Pools& pools();
  
  virtual void captureOn(const Point& p);
  virtual void move(const Move& m);
};

// IMPLEMENTATION

template <typename Board, typename Move>
GameState<Board, Move>::GameState() { }

template <typename Board, typename Move>
GameState<Board, Move>::GameState(
      typename Piece::Color turn, 
      bool wk, 
      bool wq, 
      bool bk, 
      bool bq, 
      const Point& ep)
: Base(turn, wk, wq, bk, bq, ep) { }

template <typename Board, typename Move>
const typename GameState<Board, Move>::Pools& GameState<Board, Move>::pools() const {
  return m_pools;
}

template <typename Board, typename Move>
typename GameState<Board, Move>::Pools& GameState<Board, Move>::pools() {
  return m_pools;
}

template <typename Board, typename Move>
void GameState<Board, Move>::move(const Move& m) {
  if (m.drop() == Piece()) {
    Base::move(m);
    
    if (m.promoteTo() != -1) {
      Piece promoted = m_board.get(m.to());
      promoted.setPromoted();
      m_board.set(m.to(), promoted);
    }
  }
  else {
    Piece captured = m_board.get(m.to());
    
    m_board.set(m.to(), m.drop());
    m_pools.pool(m.drop().color()).remove(m.drop().type());
    
    // handle capturing by drop: some variants
    // could use it
    if (captured != Piece())
      m_pools.pool(Piece::oppositeColor(captured.color())).add(captured.type());
      
    this->switchTurn();
  }
}

template <typename Board, typename Move>
void GameState<Board, Move>::captureOn(const Point& p) {
  Piece captured = m_board.get(p);
  if (captured != Piece()) {
    m_pools.pool(Piece::oppositeColor(captured.color()))
      .add(captured.actualType());
  }
  Base::captureOn(p);
}

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__GAMESTATE_H

