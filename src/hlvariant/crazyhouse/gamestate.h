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
  virtual const Pools& pools() const;
  virtual Pools& pools();
  
  virtual void captureOn(const Point& p);
  virtual void move(const Move& m);
};

// IMPLEMENTATION

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
    m_board.set(m.to(), m.drop());
    m_pools.pool(m.drop().color()).remove(m.drop().type());
  }
}

template <typename Board, typename Move>
void GameState<Board, Move>::captureOn(const Point& p) {
  Piece captured = m_board.get(p);
  if (captured != Piece()) {
    m_pools.pool(captured.color()).add(captured.actualType());
  }
  Base::captureOn(p);
}

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__GAMESTATE_H

