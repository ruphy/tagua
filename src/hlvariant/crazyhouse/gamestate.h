#ifndef HLVARIANT__CRAZYHOUSE__GAMESTATE_H
#define HLVARIANT__CRAZYHOUSE__GAMESTATE_H

#include "../chess/gamestate.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename _Board, typename _Move>
class GameState : public HLVariant::Chess::GameState<_Board, _Move> {
  typedef HLVariant::Chess::GameState<_Board, _Move> Base;
public:
  typedef typename Base::Board Board;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
  typedef NoPool Pool;
};

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__GAMESTATE_H

