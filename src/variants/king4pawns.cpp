/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "king4pawns.h"
#include "kboard.h"
#include "variants/chess.h"
#include "variants/xchess/position.h"
#include "moveserializer.impl.h"
#include "highlevel.h"

class King4PawnsPosition : public ChessPosition {
  int m_move_ordinal;
public:
  King4PawnsPosition();
  King4PawnsPosition(const OptList& l);
  King4PawnsPosition(const ChessPosition&);
  King4PawnsPosition(Piece::Color turn, bool wk, bool wq,
                     bool bk, bool bq, const Point& ep);
  virtual King4PawnsPosition* clone() const { return new King4PawnsPosition(*this); }

#define SET_PIECE(i,j, color, type) m_board[Point(i,j)] = Piece(color, type)
  virtual void setup() {
    // 4 pawns
    for (int i = 2; i <= 5; i++)
      SET_PIECE(i, 6, WHITE, PAWN);

    // 1 king
    SET_PIECE(4,7, WHITE, KING);

    // all black's army
    for (int i = 0; i < 8; i++)
      SET_PIECE(i, 1, BLACK, PAWN);

    SET_PIECE(0,0, BLACK, ROOK);
    SET_PIECE(1,0, BLACK, KNIGHT);
    SET_PIECE(2,0, BLACK, BISHOP);
    SET_PIECE(3,0, BLACK, QUEEN);
    SET_PIECE(4,0, BLACK, KING);
    SET_PIECE(5,0, BLACK, BISHOP);
    SET_PIECE(6,0, BLACK, KNIGHT);
    SET_PIECE(7,0, BLACK, ROOK);
  }
#undef SET_PIECE

  virtual void switchTurn() {
    if (turn() == WHITE && m_move_ordinal == 0)
      m_move_ordinal++;
    else {
      ChessPosition::switchTurn();
      m_move_ordinal = 0;
    }
  }

  virtual void setTurn(Piece::Color turn) {
    ChessPosition::setTurn(turn);
    m_move_ordinal = 0;
  }

  virtual bool testMove(Move& move) const {
    return pseudolegal(move);
  }
};

King4PawnsPosition::King4PawnsPosition()
: m_move_ordinal(0) { }
King4PawnsPosition::King4PawnsPosition(const OptList&)
: m_move_ordinal(0) { }
King4PawnsPosition::King4PawnsPosition(const ChessPosition& other)
: ChessPosition(other)
, m_move_ordinal(0) { }
King4PawnsPosition::King4PawnsPosition(Piece::Color turn, bool wk, bool wq,
                                       bool bk, bool bq, const Point& ep)
: ChessPosition(turn, wk, wq, bk, bq, ep)
, m_move_ordinal(0) { }

class King4PawnsVariantInfo {
public:
  typedef King4PawnsPosition Position;
  typedef Position::Piece Piece;
  typedef Position::Move Move;
  typedef ChessAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static int moveListLayout(){ return 2; }
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
  }
  static const char *m_name;
  static const char *m_theme_proxy;
  static OptList positionOptions() { return OptList(); }
};

const char *King4PawnsVariantInfo::m_name = "King4Pawns";
const char *King4PawnsVariantInfo::m_theme_proxy = "Chess";

void King4PawnsVariantInfo::forallPieces(PieceFunction& f) {
  ChessVariant::forallPieces(f);
}

VariantInfo* King4PawnsVariant::static_variant_info = 0;

VariantInfo* King4PawnsVariant::info() {
  if (!static_variant_info)
    static_variant_info = new WrappedVariantInfo<King4PawnsVariantInfo>;
  return static_variant_info;
}