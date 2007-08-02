/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "move.h"
#include "piece.h"
#include "usermove.h"


ChessMove::ChessMove(Point from, Point to, PieceType promotionType)
: m_type(Invalid)
, from(from), to(to)
, promotionType(promotionType)
, status(Untested) {}

ChessMove::ChessMove()
: m_type(Invalid)
, promotionType(INVALID_TYPE)
, status(Untested) {}

QString ChessMove::promotionSymbol() const {
  return ChessPiece::typeSymbol(promotionType);
}

QString ChessMove::toString(int xsize, int ysize) const {
  QString res = from.toString(ysize) + to.toString(ysize);
  if (m_type == Promotion)
    res = res + "=" + promotionSymbol();

  return res;
}

bool ChessMove::operator==(const ChessMove& other) const {
  return from == other.from
      && to == other.to
      && promotionType == other.promotionType;
}

bool ChessMove::operator!=(const ChessMove& other) const {
  return !operator==(other);
}

ChessMove ChessMove::createDropMove(int pool, int piece_index, const Point&) {
  return ChessMove();
}

