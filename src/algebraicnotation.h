/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ALGEBRAICNOTATION_H
#define ALGEBRAICNOTATION_H

#include <iosfwd>
#include "common.h"
#include "point.h"

class QRegExp;
class QString;
class GenericPosition;

class AlgebraicNotation {
  friend std::ostream& operator<<(std::ostream& os, const AlgebraicNotation& move);

  static QRegExp pattern;
  static QRegExp kingCastlingPattern;
  static QRegExp queenCastlingPattern;
  static QRegExp nonePattern;
public:
  enum CastlingType {
      NoCastling,
      KingSide,
      QueenSide
  };

  AlgebraicNotation(const QString&, int& offset, int ysize);
  explicit AlgebraicNotation(const QString&, int ysize);
  AlgebraicNotation();

  void init(const QString&, int& offset, int ysize);

  Point from, to;
  int type;
  int promotion;
  CastlingType castling;
  bool drop;

  inline bool invalid() const { return (to == Point::invalid()) && (castling == NoCastling); }
  inline bool valid() const { return !invalid(); }
};

class VerboseNotation {
  static QRegExp pattern;
  static QRegExp kingCastlingPattern;
  static QRegExp queenCastlingPattern;
  static QRegExp nonePattern;

public:
  explicit VerboseNotation(const QString&, int ysize);
  Point from, to;
  int type, promotion;
  AlgebraicNotation::CastlingType castling;
  inline bool invalid() const { return (from == Point::invalid() && to == Point::invalid()); }
  inline bool valid() const { return !invalid(); }
};

// IMPLEMENTATION


#define TRY(x) ref.getMove(x, ok); if (ok) return;
template <typename Pos>
void minimalNotation(AlgebraicNotation& san, const Pos& ref) {
  Point from = san.from;
  san.castling = AlgebraicNotation::NoCastling;
  bool ok;

  // try notation without starting point
  san.from = Point::invalid();
  TRY(san);

  // add column indication
  san.from = Point(from.x, -1);
  TRY(san);

  // add row indication
  san.from = Point(-1, from.y);
  TRY(san);

  // add complete starting point
  san.from = from;
}
#undef TRY


#endif // ALGEBRAICNOTATION_H
