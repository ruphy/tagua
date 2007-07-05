/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CHESS_H
#define CHESS_H

#include "variants/xchess/position.h"
#include "variants/xchess/animator.h"
#include "variants/xchess/piece.h"
#include "variants/xchess/move.h"
#include "option.h"
#include "piecegrid.h"

class ChessPiece;
class ChessMove;
class ChessAnimator;
class VariantInfo;
template <typename T> class PointerGrid;
template <typename M, typename P, typename B> class Position;
typedef Grid<ChessPiece> PieceGrid;
typedef Position<ChessMove, ChessPiece, PieceGrid> ChessPosition;

struct ChessVariant {
  typedef ChessPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef ChessAnimator Animator;

  static const bool m_simple_moves = false;
  static const char *m_name;
  static const char *m_theme_proxy;
  static void forallPieces(class PieceFunction& f);
  static int  moveListLayout(){ return 0; }

  static VariantInfo* static_chess_variant;
  static VariantInfo* info();
  static OptList positionOptions() { return OptList(); }
};


#endif // CHESS_H
