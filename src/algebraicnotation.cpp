/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QRegExp>
#include "algebraicnotation.h"
#include "variants/xchess/piece.h"
#include "variants/xchess/move.h"

//BEGIN AlgebraicNotation

//                                        1           2                   3
QRegExp AlgebraicNotation::pattern("^([PRNBKQ])?([a-wyzA-Z]?\\d*|x\\d+)([-x@])?"
//                                       4         5        6
                                   "([a-zA-Z]\\d+)(=?([RNBKQrnbkq]))?[+#]?[\?!]*");
QRegExp AlgebraicNotation::kingCastlingPattern("^[oO0]-?[oO0][+#]?");
QRegExp AlgebraicNotation::queenCastlingPattern("^[oO0]-?[oO0]-?[oO0][+#]?");
QRegExp AlgebraicNotation::nonePattern("^none");

AlgebraicNotation::AlgebraicNotation()
: from(Point::invalid())
, to(Point::invalid())
, type(-1)
, promotion(-1)
, castling(NoCastling)
, drop(false) {
}

AlgebraicNotation::AlgebraicNotation(const QString& str, int ysize)
: from(Point::invalid())
, to(Point::invalid())
, type(-1)
, promotion(-1)
, castling(NoCastling)
, drop(false) {

  int offset = 0;
  init(str, offset, ysize);
}

AlgebraicNotation::AlgebraicNotation(const QString& str, int& offset, int ysize)
: from(Point::invalid())
, to(Point::invalid())
, type(-1)
, promotion(-1)
, castling(NoCastling)
, drop(false) {

  init(str, offset, ysize);
}

void AlgebraicNotation::init(const QString& str, int& offset, int ysize) {
  if (nonePattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    from = Point::invalid();
    to = Point::invalid();
    offset += nonePattern.matchedLength();
  }
  else if (pattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    type = ChessPiece::getType(pattern.cap(1));
    drop = pattern.cap(3) == "@";
    if (drop)
      from = Point::invalid();
    else
      from = Point(pattern.cap(2), ysize);
    to = Point(pattern.cap(4), ysize);
    promotion = pattern.cap(6).isEmpty() ? -1 : ChessPiece::getType(pattern.cap(6));
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

std::ostream& operator<<(std::ostream& os, const AlgebraicNotation& move) {
  if (move.castling == AlgebraicNotation::KingSide)
    return os << "O-O";
  else if (move.castling == AlgebraicNotation::QueenSide)
    return os << "O-O-O";
  else
    return os << move.type << ": " << move.from << " -> " << move.to;
}

//END AlgebraicNotation


//BEGIN VerboseNotation
//                                     1          2           3            5
QRegExp VerboseNotation::pattern("([PRNBKQ])/([a-zA-Z]\\d+|@@)-([a-zA-Z]\\d+)(=([PRNBKQ]))?");
QRegExp VerboseNotation::kingCastlingPattern("[oO0]-[oO0]");
QRegExp VerboseNotation::queenCastlingPattern("[oO0]-[oO0]-[oO0]");
QRegExp VerboseNotation::nonePattern("none");

VerboseNotation::VerboseNotation(const QString& str, int ysize)
: promotion(INVALID_TYPE) {
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

    type = ChessPiece::getType(pattern.cap(1));
    if (!pattern.cap(5).isEmpty())
      promotion = ChessPiece::getType(pattern.cap(6));
    else
      promotion = INVALID_TYPE;
    castling = AlgebraicNotation::NoCastling;
  }
  else if (queenCastlingPattern.indexIn(str) == 0)
    castling = AlgebraicNotation::QueenSide;
  else if (kingCastlingPattern.indexIn(str) == 0)
    castling = AlgebraicNotation::KingSide;
  else {
    from = Point::invalid();
    to = Point::invalid();
    castling = AlgebraicNotation::NoCastling;
  }
}

//END VerboseNotation
