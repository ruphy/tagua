/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVE_H
#define MOVE_H

#include "point.h"
#include "piecetype.h"
#include "moveserializer.h"

class DropUserMove;

class ChessMove {
public:
  enum Type {
    Invalid = 0,
    Normal,
    EnPassantCapture,
    EnPassantTrigger,
    KingSideCastling,
    QueenSideCastling,
    Promotion
  };
  Type m_type;
public:
  enum Status {
    Untested,
    Legal,
    Illegal
  };
  Point from, to;
  PieceType promotionType;
  Status status;
  Type type() const { return m_type; }
  void setType(Type value) { m_type = value; }

  QString promotionSymbol() const;

  ChessMove(Point from, Point to, PieceType promotionType = INVALID_TYPE);
  ChessMove();

  static ChessMove invalid() { return ChessMove(Point::invalid(), Point::invalid()); }
  inline bool valid() const { return from != Point::invalid() && to != Point::invalid(); }
  QString toString(int ysize) const;
  bool operator==(const ChessMove& other) const;
  bool operator!=(const ChessMove& other) const;

  static ChessMove createDropMove(const class ChessPiece&, const Point&);
};

#endif // MOVE_H
