/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
	    (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__MINISHOGI__GAMESTATE_H
#define HLVARIANT__MINISHOGI__GAMESTATE_H

#include "../shogi/gamestate.h"
#include "../shogi/piece.h"

namespace HLVariant {
namespace MiniShogi {

template <typename _Board, typename _Move>
  class GameState:
  public Shogi::GameState<_Board, _Move> {
public:
  virtual void setup();
  virtual bool promotionZone(typename Shogi::Piece::Color player, const Point& p) const;
};

// IMPLEMENTATION

#define MS_COL(i, c) c == Shogi::Piece::BLACK ? (i) :		\
  (Shogi::GameState<Board, Move>::board().size().x - i - 1)
#define MS_ROW(i, c) rank + i * Shogi::GameState<Board, Move>::direction(color).y
#define MS_PIECE(x,y,p) Shogi::GameState<Board, Move>::board().set(Point(MS_COL(x, c), MS_ROW(y, c)), \
								Shogi::Piece(color, p))
template <typename Board, typename Move>
void GameState<Board, Move>::setup() {
  for (int c = 0; c < 2; c++) {
    typename Shogi::Piece::Color color = static_cast<typename Shogi::Piece::Color>(c);
    int rank = Shogi::GameState<Board, Move>::startingRank(color);

    MS_PIECE(0, 0, Shogi::Piece::KING);
    MS_PIECE(1, 0, Shogi::Piece::GOLD);
    MS_PIECE(2, 0, Shogi::Piece::SILVER);
    MS_PIECE(3, 0, Shogi::Piece::BISHOP);
    MS_PIECE(4, 0, Shogi::Piece::ROOK);

    MS_PIECE(0, 1, Shogi::Piece::PAWN);
 }
}
#undef MS_PIECE
#undef MS_ROW
#undef MS_COL

template <typename Board, typename Move>
bool GameState<Board, Move>::promotionZone(typename Shogi::Piece::Color player, const Point& p) const {
  return player == Shogi::Piece::WHITE ? p.y == 4 : p.y == 0;
}

} // namespace MiniShogi
} // namespace HLVariant

#endif // HLVARIANT__MINISHOGI__GAMESTATE_H
