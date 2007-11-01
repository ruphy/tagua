/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
	    (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__TORI_SHOGI__GAMESTATE_H
#define HLVARIANT__TORI_SHOGI__GAMESTATE_H

#include "../shogi/gamestate.h"
#include "piece.h"

namespace HLVariant {
namespace ToriShogi {

template <typename _Board, typename _Move>
  class GameState:
  public Shogi::GameState<_Board, _Move> {
public:
  virtual void setup();
  virtual bool promotionZone(typename Piece::Color player, const Point& p) const;
  virtual bool canPromote(const Piece& p) const;
};

// IMPLEMENTATION

#define MS_COL(i, c) c == Piece::BLACK ? (i) :		\
  (Shogi::GameState<Board, Move>::board().size().x - i - 1)
#define MS_ROW(i, c) rank + i * Shogi::GameState<Board, Move>::direction(color).y
#define MS_PIECE(x,y,p) Shogi::GameState<Board, Move>::board().set(Point(MS_COL(x, c), MS_ROW(y, c)), \
								Piece(color, p))
template <typename Board, typename Move>
void GameState<Board, Move>::setup() {
  for (int c = 0; c < 2; c++) {
    typename Piece::Color color = static_cast<typename Piece::Color>(c);
    int rank = Shogi::GameState<Board, Move>::startingRank(color);

    MS_PIECE(0, 0, ToriShogi::Piece::LEFT_QUAIL);
    MS_PIECE(1, 0, ToriShogi::Piece::PHEASANT);
    MS_PIECE(2, 0, ToriShogi::Piece::CRANE);
    MS_PIECE(3, 0, ToriShogi::Piece::PHOENIX);
    MS_PIECE(4, 0, ToriShogi::Piece::CRANE);
    MS_PIECE(5, 0, ToriShogi::Piece::PHEASANT);
    MS_PIECE(6, 0, ToriShogi::Piece::RIGHT_QUAIL);

    MS_PIECE(3, 1, ToriShogi::Piece::FALCON);

    for (int i = 0; i < Shogi::GameState<Board,Move>::board().size().x; i++) {
      MS_PIECE(i, 2, ToriShogi::Piece::SWALLOW);
    }
    MS_PIECE(4, 3, ToriShogi::Piece::SWALLOW);
 }
}
#undef MS_PIECE
#undef MS_ROW
#undef MS_COL

template <typename Board, typename Move>
bool GameState<Board, Move>::promotionZone(typename Piece::Color player, const Point& p) const {
  return player == Piece::WHITE ? p.y >= 5 : p.y <= 1;
}

template <typename Board, typename Move>
bool GameState<Board, Move>::canPromote(const Piece& p) const {
  if (p.promoted())
    return false;

  if ((p.type() == Piece::SWALLOW) || (p.type() == Piece::FALCON))
    return true;
  else
    return false;
}

} // namespace MiniShogi
} // namespace HLVariant

#endif // HLVARIANT__TORI_SHOGI__GAMESTATE_H
