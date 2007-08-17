/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSAPI_IMPL_H
#define ICSAPI_IMPL_H

#include "tagua_wrapped.h"
#include "piecefactory.h"

template <typename Variant>
PositionPtr WrappedICSAPI<Variant>::createChessboard(
                int turn,
                bool wkCastle,
                bool wqCastle,
                bool bkCastle,
                bool bqCastle,
                const Point& ep) {
  return PositionPtr(
    new WrappedPosition<Variant>(typename Variant::Position(
      static_cast<typename Variant::Piece::Color>(turn),
      wkCastle, wqCastle, bkCastle, bqCastle, ep)));
}
  
  

template <typename Variant>
PiecePtr WrappedICSAPI<Variant>::createPiece(const QString& description) {
  return AbstractPiece::Ptr(new WrappedPiece<Variant>(
    PieceFactory<Variant>::createPiece(description)));
}


#endif // ICSAPI_IMPL_H

