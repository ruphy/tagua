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
  Pools m_pools;
public:
  const Pools& pools() const;
  Pools& pools();
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

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__GAMESTATE_H

