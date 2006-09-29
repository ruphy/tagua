/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "crazyhouse_p.h"
#include "dummy.h"
#include "piecesprite.h"

typedef CrazyhousePiece DummyPiece;
typedef CrazyhouseMove DummyMove;
typedef CrazyhouseAnimator DummyAnimator;

//BEGIN DummyPosition

class DummyPosition : public CrazyhousePosition {
public:
  friend class MoveSerializer<DummyPosition>;
  typedef DummyMove Move;
  typedef DummyPiece Piece;
  typedef CrazyhousePosition Base;
  typedef std::map<DummyPiece, int> Pool;

  DummyPosition();
  DummyPosition(const OptList& l);
  DummyPosition(const DummyPosition&);
  DummyPosition(const ChessPosition&);
  DummyPosition(DummyPiece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  virtual DummyPosition* clone() const;

public:
  virtual bool pseudolegal(Move&) const;
  virtual bool testMove(Move&) const;
  virtual void move(const Move&);
  virtual void executeCaptureOn(const Point& point) {
    this->CrazyhousePosition::Base::executeCaptureOn(point);
  }

  virtual bool operator==(const DummyPosition& other) const;

  static Move getVerboseMove(Color turn, const VerboseNotation& m) {
    Move retv = ChessPosition::getVerboseMove(turn, m);
    if(retv.from == Point::invalid())
      retv.m_drop = DummyPiece(turn, static_cast<ChessPiece::Type>(m.type) );
    else
      retv.m_drop = DummyPiece(INVALID_COLOR, INVALID_TYPE);
    return retv;
  }
};

DummyPosition::DummyPosition() {
}

DummyPosition::DummyPosition(const OptList&) {
}

DummyPosition::DummyPosition(const DummyPosition& other)
: Base(other) {
}

DummyPosition::DummyPosition(const ChessPosition& other)
: Base(other) {
}

DummyPosition::DummyPosition(DummyPiece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep)
: Base(turn, wk, wq, bk, bq, ep) {
}

DummyPosition* DummyPosition::clone() const {
  return new DummyPosition(*this);
}

bool DummyPosition::pseudolegal(Move& move) const {
  if (move.m_drop.valid())
    return valid(move.to);
  else
    return valid(move.from) && valid(move.to);
}

bool DummyPosition::testMove(Move& move) const {
  if (move.status == Move::Untested)
    move.status = pseudolegal(move) ? Move::Legal : Move::Illegal;
  return move.status == Move::Legal;
}

void DummyPosition::move(const Move& move) {
  if (move.m_drop.valid()) {
    basicDropPiece(new Piece(move.m_drop), move.to);
    if(--m_pool[move.m_drop] <= 0)
      m_pool.erase(move.m_drop);
  }
  else {
    executeCaptureOn(move.to);
    basicMovePiece(move);
    switchTurn();
  }
}

bool DummyPosition::operator==(const DummyPosition& pos) const {
  return pool() == pos.pool() && Base::operator==(pos);
}

//END DummyPosition


//BEGIN DummyVariant


#include "highlevel.h"
#include "moveserializer.impl.h"

template <>
class MoveSerializer<DummyPosition> : public AbstractMoveSerializer {
  const DummyMove& m_move;
  const DummyPosition& m_ref;
public:
  MoveSerializer(const DummyMove& m, const DummyPosition& r)
  : m_move(m), m_ref(r) { }

  virtual QString SAN() const {
    if(m_move.m_drop.valid())
      return CrazyhousePiece::typeSymbol(m_move.m_drop.type()) + "@" + m_move.to.toString(8);
    else {
      QString res = m_move.from.toString(m_ref.size().y) + m_move.to.toString(m_ref.size().y);
      if (m_move.m_type == DummyMove::Promotion)
        res = res + "=" + m_move.promotionSymbol();
      if(m_ref.m_board.valid(m_move.from) && m_ref.m_board[m_move.from]
            && m_ref.m_board[m_move.from]->type() != PAWN)
          res = DummyPiece::typeSymbol(m_ref.m_board[m_move.from]->type()) + res;
      return res;
    }
  }
};

class DummyVariantInfo {
public:
  typedef DummyPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef DummyAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static int moveListLayout(){ return 0; }
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
  }
  static const char *m_name;
  static const char *m_theme_proxy;
  static OptList positionOptions() { return OptList(); }
};

const char *DummyVariantInfo::m_name = "Dummy";
const char *DummyVariantInfo::m_theme_proxy = "Chess";

VariantInfo* DummyVariant::static_dummy_variant = 0;

void DummyVariantInfo::forallPieces(PieceFunction& f) {
  return ChessVariant::forallPieces(f);
}

VariantInfo* DummyVariant::info() {
  if (!static_dummy_variant)
    static_dummy_variant = new WrappedVariantInfo<DummyVariantInfo>;
  return static_dummy_variant;
}

//END DummyVariant
