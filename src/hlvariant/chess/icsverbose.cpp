/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "icsverbose.h"

namespace HLVariant {
namespace Chess {

//                               1          2           3            5
QRegExp ICSVerbose::pattern("([PRNBKQ])/([a-zA-Z]\\d+|@@)-([a-zA-Z]\\d+)(=([PRNBKQ]))?");
QRegExp ICSVerbose::kingCastlingPattern("[oO0]-[oO0]");
QRegExp ICSVerbose::queenCastlingPattern("[oO0]-[oO0]-[oO0]");
QRegExp ICSVerbose::nonePattern("none");

ICSVerbose::ICSVerbose()
: from(Point::invalid())
, to(Point::invalid())
, promotion(-1)
, castling(SAN::NoCastling) { }


void ICSVerbose::load(const QString& str, int ysize) {
  if (nonePattern.indexIn(str) == 0) {
    from = Point::invalid();
    to = Point::invalid();
  }
  else if (pattern.indexIn(str) == 0) {
    if (pattern.cap(2) == "@@")
      from = Point::invalid();
    else
      from = Point(pattern.cap(2), ysize);

    to = Point(pattern.cap(3), ysize);

    type = SAN::getType(pattern.cap(1));
    if (!pattern.cap(5).isEmpty())
      promotion = SAN::getType(pattern.cap(6));
    else
      promotion = -1;
    castling = SAN::NoCastling;
  }
  else if (queenCastlingPattern.indexIn(str) == 0)
    castling = SAN::QueenSide;
  else if (kingCastlingPattern.indexIn(str) == 0)
    castling = SAN::KingSide;
  else {
    from = Point::invalid();
    to = Point::invalid();
    castling = SAN::NoCastling;
  }
}

} // namespace HLVariant
} // namespace Chess

