/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVESERIALIZER_IMPL_H
#define MOVESERIALIZER_IMPL_H

#include "moveserializer.h"
#include "algebraicnotation.h"

/**
 * Assume the move has already been tested against @a ref.
 * Calling this function on an untested or illegal move is safe but its return
 * value is undefined.
 * Do not try to call this function on an invalid move.
 * @param ref The position in which this move shall be executed.
 * @return A compact SAN representation for this move.
 */
template <typename Pos>
QString MoveSerializerBase<Pos>::SAN() const {
  Q_ASSERT(m_move.valid());

  Piece piece = m_ref.get(m_move.from);
  Piece captured = m_ref.get(m_move.to);

  if (!piece) {
    m_ref.dump();
    std::cout << "Move is: " << m_move.toString(m_ref.size().x, m_ref.size().y) << std::endl;
  }

  Q_ASSERT(piece);
  QString res;
  if (piece.type() == PAWN) {
    if (captured || m_move.type() == Move::EnPassantCapture)
      res = m_move.from.col() + "x";

    res += m_move.to.toString(m_ref.size().y);
  }

  else {
    if (m_move.type() == Move::KingSideCastling) {
      res = "O-O";
    }
    else if (m_move.type() == Move::QueenSideCastling) {
      res = "O-O-O";
    }
    else {
      res = Piece::typeSymbol(piece.type());

      AlgebraicNotation temp;
      temp.from = m_move.from;
      temp.to = m_move.to;
      temp.type = piece.type();
      temp.castling = AlgebraicNotation::NoCastling;
      minimalNotation(temp, m_ref);

      res += temp.from.toString(m_ref.size().y);
      if (captured) res += "x";
      res += temp.to.toString(m_ref.size().y);
    }
  }

  if (m_move.type() == Move::Promotion)
    res += "=" + Piece::typeSymbol(m_move.promotionType);

  return res + checkSuffix();
}

template <typename Pos>
DecoratedMove MoveSerializerBase<Pos>::toDecoratedMove() const {
  static QRegExp reg("[KQRBNP]");
  QString move = SAN();
  DecoratedMove mv;
  int x = 0;
  while(reg.indexIn(move, x) != -1) {
    if(reg.pos() > x)
      mv.push_back(MovePart(move.mid(x, reg.pos()-x)));
    switch(move[reg.pos()].toAscii()) {
      case 'K':
        mv.push_back(MovePart("king", MovePart::Figurine));
        break;
      case 'Q':
        mv.push_back(MovePart("queen", MovePart::Figurine));
        break;
      case 'R':
        mv.push_back(MovePart("rook", MovePart::Figurine));
        break;
      case 'B':
        mv.push_back(MovePart("bishop", MovePart::Figurine));
        break;
      case 'N':
        mv.push_back(MovePart("knight", MovePart::Figurine));
        break;
      case 'P':
        mv.push_back(MovePart("pawn", MovePart::Figurine));
        break;
    }
    x = reg.pos() + reg.matchedLength();
  }
  if(x<move.length())
    mv.push_back(MovePart(move.mid(x)));
  return mv;
}

#endif // MOVESERIALIZER_IMPL_H
