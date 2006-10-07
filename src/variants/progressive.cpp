/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "variants/progressive.h"
#include "kboard.h"
#include "variants/chess.h"
#include "variants/xchess/position.h"
#include "moveserializer.impl.h"
#include "highlevel.h"

class ProgressivePosition : public ChessPosition {
  int m_progression;
  int m_move_ordinal;
public:
  ProgressivePosition();
  ProgressivePosition(const OptList& l);
  ProgressivePosition(const ChessPosition&);
  ProgressivePosition(Piece::Color turn, bool wk, bool wq,
                     bool bk, bool bq, const Point& ep);
  virtual ProgressivePosition* clone() const { return new ProgressivePosition(*this); }

  virtual void switchTurn() {
    m_move_ordinal++;

    if(m_move_ordinal >= m_progression || check(Piece::oppositeColor(turn()))) {
      m_progression++;
      m_move_ordinal = 0;
      ChessPosition::switchTurn();
    }
  }

  virtual void setTurn(Piece::Color turn) {
    ChessPosition::setTurn(turn);
    m_progression = 1;
    m_move_ordinal = 0;
  }
};

ProgressivePosition::ProgressivePosition()
: m_progression(1)
, m_move_ordinal(0) { }
ProgressivePosition::ProgressivePosition(const OptList& /*l*/)
: m_progression(1)
, m_move_ordinal(0) { }
ProgressivePosition::ProgressivePosition(const ChessPosition& other)
: ChessPosition(other)
, m_progression(1)
, m_move_ordinal(0) { }
ProgressivePosition::ProgressivePosition(Piece::Color turn, bool wk, bool wq,
                                       bool bk, bool bq, const Point& ep)
: ChessPosition(turn, wk, wq, bk, bq, ep)
, m_progression(1)
, m_move_ordinal(0) { }

class ProgressiveVariantInfo {
public:
  typedef ProgressivePosition Position;
  typedef Position::Piece Piece;
  typedef Position::Move Move;
  typedef ChessAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static int moveListLayout(){ return 4; }
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
  }
  static const char *m_name;
  static const char *m_theme_proxy;
  static OptList positionOptions() { return OptList(); }
};

const char *ProgressiveVariantInfo::m_name = "Progressive";
const char *ProgressiveVariantInfo::m_theme_proxy = "Chess";

void ProgressiveVariantInfo::forallPieces(PieceFunction& f) {
  ChessVariant::forallPieces(f);
}

VariantInfo* ProgressiveVariant::static_variant_info = 0;

VariantInfo* ProgressiveVariant::info() {
  if (!static_variant_info)
    static_variant_info = new WrappedVariantInfo<ProgressiveVariantInfo>;
  return static_variant_info;
}