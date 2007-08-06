/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOGI__GAMESTATE_H
#define HLVARIANT__SHOGI__GAMESTATE_H

#include "../crazyhouse/gamestate.h"
#include "../pool.h"
#include "../poolcollection.h"

namespace HLVariant {
namespace Shogi {

template <typename _Board, typename _Move>
class GameState {
public:
  typedef _Board Board;
  typedef _Move Move;
  typedef typename Board::Piece Piece;
  typedef Pool<Piece> Pool;
  typedef PoolCollection<Pool> Pools;
private:
  Board m_board;
  Pools m_pools;
  typename Piece::Color m_turn;
public:
  GameState();
  virtual ~GameState();

  virtual Board& board();
  virtual const Board& board() const;
  
  virtual Pools& pools();
  virtual const Pools& pools() const;
  
  virtual void setup();
  
  virtual bool operator==(const GameState<Board, Move>& other) const;
  
  virtual void move(const Move& m);
  virtual void basicMove(const Move& m);
  virtual void captureOn(const Point& p);
  virtual bool promotionZone(typename Piece::Color player, const Point& p) const;
  virtual bool canPromote(const Piece& p) const;
  
  virtual void setTurn(typename Piece::Color color);
  virtual typename Piece::Color previousTurn() const;
  virtual void switchTurn();
  virtual typename Piece::Color turn() const;
  
  virtual int startingRank(typename Piece::Color color) const;
  virtual int promotionRank(typename Piece::Color color) const;
  virtual Point kingStartingPosition(typename Piece::Color color) const;
  virtual Point direction(typename Piece::Color color) const;
};

// IMPLEMENTATION

template <typename Board, typename Move>
GameState<Board, Move>::GameState()
: m_turn(Piece::BLACK) { }

template <typename Board, typename Move>
GameState<Board, Move>::~GameState() { }

template <typename Board, typename Move>
Board& GameState<Board, Move>::board() { return m_board; }

template <typename Board, typename Move>
const Board& GameState<Board, Move>::board() const { return m_board; }

template <typename Board, typename Move>
typename GameState<Board, Move>::Pools& GameState<Board, Move>::pools() { return m_pools; }

template <typename Board, typename Move>
const typename GameState<Board, Move>::Pools& GameState<Board, Move>::pools() const { return m_pools; }

#define COL(i, c) c == Piece::BLACK ? (i) : (m_board.size().x - i - 1)
template <typename Board, typename Move>
void GameState<Board, Move>::setup() {
  for (int c = 0; c < 2; c++) {
    typename Piece::Color color = static_cast<typename Piece::Color>(c);
    int rank = startingRank(color);
    for (int i = 0; i < m_board.size().x; i++) {
      m_board.set(Point(i, rank + direction(color).y * 2), 
                  Piece(color, Piece::PAWN));
    }
    m_board.set(Point(0, rank), Piece(color, Piece::LANCE));
    m_board.set(Point(1, rank), Piece(color, Piece::KNIGHT));
    m_board.set(Point(2, rank), Piece(color, Piece::SILVER));
    m_board.set(Point(3, rank), Piece(color, Piece::GOLD));
    m_board.set(Point(4, rank), Piece(color, Piece::KING));
    m_board.set(Point(5, rank), Piece(color, Piece::GOLD));
    m_board.set(Point(6, rank), Piece(color, Piece::SILVER));
    m_board.set(Point(7, rank), Piece(color, Piece::KNIGHT));
    m_board.set(Point(8, rank), Piece(color, Piece::LANCE));
    
    m_board.set(Point(COL(1, c), rank + direction(color).y), 
      Piece(color, Piece::BISHOP));
    m_board.set(Point(COL(7, c), rank + direction(color).y), 
      Piece(color, Piece::ROOK));
  }
}
#undef COL

template <typename Board, typename Move>
bool GameState<Board, Move>::promotionZone(typename Piece::Color player, const Point& p) const {
  return player == Piece::WHITE ? p.y >= 6 : p.y <= 2;
}

template <typename Board, typename Move>
bool GameState<Board, Move>::canPromote(const Piece& p) const {
  if (p.promoted())
    return false;

  if ((p.type() == Piece::KING) || (p.type() == Piece::GOLD))
    return false;
  else
    return true;
}

template <typename Board, typename Move>
bool GameState<Board, Move>::operator==(const GameState<Board, Move>& other) const {
  return m_turn == other.m_turn && 
    m_board == other.m_board;
}

template <typename Board, typename Move>
void GameState<Board, Move>::move(const Move& m) {
  if (m.drop() == Piece()) {
    captureOn(m.to());
    basicMove(m);
    
    if (m.promoteTo() != -1) {
      Piece piece = m_board.get(m.to());
      piece.setPromoted();
      m_board.set(m.to(), piece);
    }
  }
  else {
    m_board.set(m.to(), m.drop());
    m_pools.pool(m.drop().color()).remove(m.drop().type());
  }
  
  switchTurn();
}

template <typename Board, typename Move>
void GameState<Board, Move>::captureOn(const Point& p) {
  Piece captured = m_board.get(p);
  m_board.set(p, Piece());
  m_pools.pool(Piece::oppositeColor(captured.color())).add(captured.type());
}

template <typename Board, typename Move>
void GameState<Board, Move>::basicMove(const Move& m) {
  if (m.from() != m.to()) {
    m_board.set(m.to(), m_board.get(m.from()));
    m_board.set(m.from(), Piece());
  }
}

template <typename Board, typename Move>
void GameState<Board, Move>::switchTurn() {
  m_turn = Piece::oppositeColor(m_turn);
}

template <typename Board, typename Move>
typename Board::Piece::Color GameState<Board, Move>::turn() const {
  return m_turn;
}

template <typename Board, typename Move>
typename Board::Piece::Color GameState<Board, Move>::previousTurn() const {
  return Piece::oppositeColor(m_turn);
}

template <typename Board, typename Move>
void GameState<Board, Move>::setTurn(typename Piece::Color color) {
  m_turn = color;
}

template <typename Board, typename Move>
int GameState<Board, Move>::startingRank(typename Piece::Color color) const {
  return color == Piece::BLACK ? m_board.size().y - 1 : 0;
}

template <typename Board, typename Move>
int GameState<Board, Move>::promotionRank(typename Piece::Color color) const {
  return startingRank(Piece::oppositeColor(color));
}

template <typename Board, typename Move>
Point GameState<Board, Move>::kingStartingPosition(typename Piece::Color color) const {
  return Point(4, startingRank(color));
}

template <typename Board, typename Move>
Point GameState<Board, Move>::direction(typename Piece::Color color) const {
  return Point(0, color == Piece::BLACK ? -1 : 1);
}


} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__GAMESTATE_H
