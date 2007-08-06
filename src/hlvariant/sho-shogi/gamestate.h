/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
	    (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOSHOGI__GAMESTATE_H
#define HLVARIANT__SHOSHOGI__GAMESTATE_H

#include "../shogi/gamestate.h"
#include "../shogi/piece.h"

namespace HLVariant {
namespace ShoShogi {

template <typename _Board, typename _Move>
  class GameState:
  public Shogi::GameState<_Board, _Move> {
public:
  virtual void setup();
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

    for (int i = 0; i < Shogi::GameState<Board, Move>::board().size().x; i++) {
      MS_PIECE(i, 2, Shogi::Piece::PAWN);
    }
    MS_PIECE(0, 0, Shogi::Piece::LANCE);
    MS_PIECE(1, 0, Shogi::Piece::KNIGHT);
    MS_PIECE(2, 0, Shogi::Piece::SILVER);
    MS_PIECE(3, 0, Shogi::Piece::GOLD);
    MS_PIECE(4, 0, Shogi::Piece::KING);
    MS_PIECE(5, 0, Shogi::Piece::GOLD);
    MS_PIECE(6, 0, Shogi::Piece::SILVER);
    MS_PIECE(7, 0, Shogi::Piece::KNIGHT);
    MS_PIECE(8, 0, Shogi::Piece::LANCE);

    MS_PIECE(1, 1, Shogi::Piece::BISHOP);
    MS_PIECE(4, 1, Shogi::Piece::DRUNKEN_ELEPHANT);
    MS_PIECE(7, 1, Shogi::Piece::ROOK);
 }
}
#undef MS_PIECE
#undef MS_ROW
#undef MS_COL

} // namespace ShoShogi
} // namespace HLVariant

#endif // HLVARIANT__SHOSHOGI__GAMESTATE_H
