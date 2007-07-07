/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECE_H
#define PIECE_H

#include <iostream>

#include "point.h"
#include "move.h"
#include "piecetype.h"
#include "common.h"
#include "pathinfo.h"


class ChessPiece {
public:
  typedef PieceType Type;
  typedef Type PromotionType;
  typedef PieceColor Color;
protected:
  typedef ChessMove Move;
private:
  Color m_color;
  Type m_type;
public:
  ChessPiece(Color = INVALID_COLOR, Type = INVALID_TYPE);
  ChessPiece(const ChessPiece& p);
  ChessPiece& operator=(const ChessPiece& p);
  ChessPiece* clone() const;

  virtual ~ChessPiece() {}
  inline static Color oppositeColor(Color color) { return color == WHITE ? BLACK :
                                              color == BLACK ? WHITE : INVALID_COLOR; }

  inline Color color() const { return m_color; }
  inline Type type() const { return m_type; }
  inline bool valid() const { return m_color != INVALID_COLOR &&
                                          m_type != INVALID_TYPE; }
  QString typeName() const;
  QString name() const;

  template <typename Pos>
  Move::Type canMove(const Pos& position, Point from, Point to) const;

  bool equals(const ChessPiece& other) const;
  bool sameColor(const ChessPiece& other) const;
  int id() const;
  static Color colorFromId(int);
  static Type typeFromId(int);

  static Type getType(const QString& str);
  static QString typeSymbol(Type type);

  inline Point kingStartingPosition() const { return Point(4, color() == WHITE? 7 : 0); }
  inline int startingRank() const { return color() == WHITE? 6 : 1; }
  inline Point direction() const { return direction(color()); }
  static Point direction(Color color) { return Point(0, color == WHITE? -1 : 1); }
  inline int promotionRank() const { return color() == WHITE? 0 : 7; }
  bool operator==(const ChessPiece& p) const;
  bool operator!=(const ChessPiece& p) const { return !operator==(p); }
  bool operator!() const { return !valid(); }
  bool operator<(const ChessPiece& p) const {
                          return m_color != p.m_color ?
                                 m_color < p.m_color :
                                 m_type < p.m_type; }
  operator bool() const { return valid(); }
};

std::ostream& operator<<(std::ostream& os, const ChessPiece& p);

template <typename Pos>
ChessPiece::Move::Type ChessPiece::canMove(const Pos& position,
                                         Point from, Point to) const {
  switch (m_type)
  {

  case ROOK:
  {
      if (from == to)
          return Move::Invalid;
      PathInfo path = position.path(from, to);
      if (path.parallel() && path.clear() && !sameColor(position[to]))
          return Move::Normal;
      else
          return Move::Invalid;
  }

  case BISHOP:
  {
      if (from == to)
          return Move::Invalid;
      PathInfo path = position.path(from, to);
      if (path.diagonal() && path.clear() && !sameColor(position[to]))
          return Move::Normal;
      else
          return Move::Invalid;
  }

  case KNIGHT:
      if (sameColor(position[to]))
          return Move::Invalid;
      else
      {
          Point delta = from - to;
          if (abs(delta.x) == 1 && abs(delta.y) == 2)
              return Move::Normal;
          if (abs(delta.y) == 1 && abs(delta.x) == 2)
              return Move::Normal;
          return Move::Invalid;
      }

  case QUEEN:
  {
      if (from == to)
          return Move::Invalid;
      PathInfo path = position.path(from, to);
      if (path.valid() && path.clear() && !sameColor(position[to]))
          return Move::Normal;
      else
          return Move::Invalid;
  }

  case KING:
  {
    if (from == to)
      return Move::Invalid;
    Point delta = to - from;
    if (abs(delta.x) <= 1 && abs(delta.y) <= 1 && !sameColor(position[to]))
      return Move::Normal;
    else if (from == kingStartingPosition()) {
      if (delta == Point(2,0)) {
        if (!position[from + Point(1,0)] &&
                !position[to] &&
                position.castleKing(color()))
            return Move::KingSideCastling;
      }
      else if (delta == Point(-2,0)) {
        if (!position[from - Point(1,0)] &&
            !position[from - Point(2,0)] &&
            !position[to] &&
            position.castleQueen(color()))
          return Move::QueenSideCastling;
      }
    }
    return Move::Invalid;
  }

  case PAWN:
  {
    ChessPiece destinationPiece = position[to];
    Point delta = to - from;
    bool enPassant = position.enPassantSquare() == to;

    // moving
    if (!destinationPiece && !enPassant) {
      if (delta == direction())
        if (to.y == promotionRank())
          return Move::Promotion;
        else
          return Move::Normal;
      if (from.y == startingRank() && delta == direction() * 2
          && !position[from + direction()])
        return Move::EnPassantTrigger;
      else
        return Move::Invalid;
    }

    // capturing
    else if (enPassant || !sameColor(destinationPiece))
      if (delta.y == direction().y && abs(delta.x) == 1)
        if (enPassant)
          return Move::EnPassantCapture;
        else if (to.y == promotionRank())
          return Move::Promotion;
        else
          return Move::Normal;

    return Move::Invalid;
  }

  default:
    return Move::Invalid;
  }
}

#endif // PIECE_H
