/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "san.h"
#include "piece.h"

namespace HLVariant {
namespace Chess {

//                         1           2                   3
QRegExp SAN::pattern("^([PRNBKQ])?([a-wyzA-Z]?\\d*|x\\d+)([-x@])?"
//                         4         5        6
                     "([a-zA-Z]\\d+)(=?([RNBKQrnbkq]))?[+#]?[\?!]*");
QRegExp SAN::kingCastlingPattern("^[oO0]-?[oO0][+#]?");
QRegExp SAN::queenCastlingPattern("^[oO0]-?[oO0]-?[oO0][+#]?");
QRegExp SAN::nonePattern("^none");

SAN::SAN()
: from(Point::invalid())
, to(Point::invalid())
, type(-1)
, promotion(-1)
, castling(NoCastling)
, drop(false) {
}

int SAN::getType(const QString& letter) {
  if (letter.isEmpty())
    return Piece::PAWN;
    
  QChar c = letter[0].toLower();
  if (c == 'k')
    return Piece::KING;
  else if (c == 'q')
    return Piece::QUEEN;
  else if (c == 'r')
    return Piece::ROOK;
  else if (c == 'n')
    return Piece::KNIGHT;
  else if (c == 'b')
    return Piece::BISHOP;
  else if (c == 'p')
    return Piece::PAWN;
  else
    return Piece::INVALID_TYPE;
}

void SAN::load(const QString& str, int& offset, int ysize) {
  if (nonePattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    from = Point::invalid();
    to = Point::invalid();
    offset += nonePattern.matchedLength();
  }
  else if (pattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    type = getType(pattern.cap(1));
    drop = pattern.cap(3) == "@";
    if (drop)
      from = Point::invalid();
    else
      from = Point(pattern.cap(2), ysize);
    to = Point(pattern.cap(4), ysize);
    promotion = pattern.cap(6).isEmpty() ? -1 : getType(pattern.cap(6));
    castling = NoCastling;
    offset += pattern.matchedLength();
  }
  else if (queenCastlingPattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    castling = QueenSide;

    offset += queenCastlingPattern.matchedLength();
  }
  else if (kingCastlingPattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    castling = KingSide;

    offset += kingCastlingPattern.matchedLength();
  }
  else {
    //std::cout << "error!!!! " << str.mid(offset) << std::endl;
    to = Point::invalid();
  }
}

void SAN::load(const QString& str, int ysize) {
  int offset = 0;
  load(str, offset, ysize);
}

std::ostream& operator<<(std::ostream& os, const SAN& move) {
  if (move.castling == SAN::KingSide)
    return os << "O-O";
  else if (move.castling == SAN::QueenSide)
    return os << "O-O-O";
  else
    return os << move.type << ": " << move.from << " -> " << move.to;
}


} // namespace Chess
} // namespace HLVariant


