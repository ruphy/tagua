#ifndef HLVARIANT__CRAZYHOUSE__MOVEFACTORY_H
#define HLVARIANT__CRAZYHOUSE__MOVEFACTORY_H

#include "../movefactory.h"

namespace HLVariant {
namespace Crazyhouse {

template <typename GameState>
class MoveFactory : public HLVariant::MoveFactory<GameState> {
public:
  typedef typename HLVariant::MoveFactory<GameState> Base;
  typedef typename Base::Move Move;
  typedef typename Base::Piece Piece;
  
  virtual Move createDropMove(const DropUserMove& drop) {
    return Move(static_cast<typename Piece::Color>(drop.pool), 
                drop.piece_index, 
                drop.to);
  }
};

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__MOVEFACTORY_H

