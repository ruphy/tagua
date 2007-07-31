/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CHESS__MOVEGENERATOR_H
#define HLVARIANT__CHESS__MOVEGENERATOR_H

#include "legalitycheck.h"

namespace HLVariant {
namespace Chess {

template <typename _LegalityCheck>
class MoveGenerator {
public:
  typedef _LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Move Move;
  typedef typename GameState::Board::Piece Piece;
  
  class MoveCallback {
  public:
    virtual ~MoveCallback() { }
    virtual bool operator()(const Move&) = 0;
  };
private:
  const GameState& m_state;
protected:
  virtual bool addMove(const Move& m, MoveCallback&) const;
  virtual bool addAllPromotions(const Move& m, MoveCallback&) const;
  virtual bool generateSlide(const Point& p, const Point& dir, MoveCallback&) const;
public:
  MoveGenerator(const GameState& state);
  virtual ~MoveGenerator();
  
  virtual bool check(typename Piece::Color) const;
  virtual bool stalled() const;
  virtual void generate(MoveCallback&) const;
  virtual bool generateFrom(const Point& p, MoveCallback&) const;
};

// IMPLEMENTATION

template <typename LegalityCheck>
MoveGenerator<LegalityCheck>::MoveGenerator(const GameState& state)
: m_state(state) { }

template <typename LegalityCheck>
MoveGenerator<LegalityCheck>::~MoveGenerator() { }

template <typename LegalityCheck>
bool MoveGenerator<LegalityCheck>::check(typename Piece::Color turn) const {
  Point kingPosition = m_state.board().find(turn, Piece::KING);
  if (!kingPosition.valid()) {
    // a missing king is considered in check
    return true;
  }
  else {
    LegalityCheck check(m_state);
    return check.attacks(Piece::oppositeColor(turn), kingPosition);
  }
}

template <typename LegalityCheck>
bool MoveGenerator<LegalityCheck>::stalled() const {
  class FindMove : public MoveCallback {
    bool m_found;
  public:
    FindMove() : m_found(false) { }
    virtual bool operator()(const Move&) { m_found = true; return false; }
    
    bool found() const { return m_found; }
  };
  
  FindMove findMove;
  generate(findMove);
  return !findMove.found();
}

template <typename LegalityCheck>
void MoveGenerator<LegalityCheck>::generate(MoveCallback& callback) const {
  for (int i = 0; i < m_state.board().size().x; i++) {
    for (int j = 0; j < m_state.board().size().y; j++) {
      if (!generateFrom(Point(i, j), callback))
        return false;
    }
  }
}

template <typename LegalityCheck>
bool MoveGenerator<LegalityCheck>::generateFrom(const Point& p, MoveCallback& callback) const {
  Piece piece = m_state.board().get(p);
  if (piece.color() == m_state.turn()) {
    switch (piece.type()) {
    case Piece::PAWN:
      {
        Point dir = m_state.direction(piece.color());
        if ((p + dir).y == m_state.promotionRank(piece.color())) {
          return addAllPromotions(Move(p, p + dir), callback) &&
          addAllPromotions(Move(p, p + dir + Point(1, 0)), callback) &&
          addAllPromotions(Move(p, p + dir + Point(-1, 0)), callback);
        }
        else {
          return addMove(Move(p, p + dir), callback) &&
          addMove(p, p + dir * 2) &&
          addMove(p, p + dir + Point(1, 0)) &&
          addMove(p, p + dir - Point(1, 0));
        }
      }
    case Piece::KNIGHT:
      return addMove(p, p + Point(1, 2), callback) &&
      addMove(p, p + Point(1, -2), callback) &&
      addMove(p, p + Point(-1, 2), callback) &&
      addMove(p, p + Point(-1, -2), callback) &&
      addMove(p, p + Point(2, 1), callback) &&
      addMove(p, p + Point(2, -1), callback) &&
      addMove(p, p + Point(-2, 1), callback) &&
      addMove(p, p + Point(-2, -1), callback);
    case Piece::BISHOP:
      return generateSlide(p, Point(1, 1), callback) &&
      generateSlide(p, Point(1, -1), callback) &&
      generateSlide(p, Point(-1, -1), callback) &&
      generateSlide(p, Point(-1, 1), callback);
    case Piece::ROOK:
      return generateSlide(p, Point(1, 0), callback) &&
      generateSlide(p, Point(0, 1), callback) &&
      generateSlide(p, Point(-1, 0), callback) &&
      generateSlide(p, Point(0, -1), callback);
    case Piece::QUEEN:
      return generateSlide(p, Point(1, 0), callback) &&
      generateSlide(p, Point(0, 1), callback) &&
      generateSlide(p, Point(-1, 0), callback) &&
      generateSlide(p, Point(0, -1), callback) &&
      generateSlide(p, Point(1, 1), callback) &&
      generateSlide(p, Point(1, -1), callback) &&
      generateSlide(p, Point(-1, -1), callback) &&
      generateSlide(p, Point(-1, 1), callback);
    case Piece::KING:
      return addMove(p, p + Point(1,0), callback) &&
      addMove(p, p + Point(1,1), callback) &&
      addMove(p, p + Point(0,1), callback) &&
      addMove(p, p + Point(-1,1), callback) &&
      addMove(p, p + Point(-1,0), callback) &&
      addMove(p, p + Point(-1,-1), callback) &&
      addMove(p, p + Point(0,-1), callback) &&
      addMove(p, p + Point(1,-1), callback);
    default:
      return true;
    }
  }
  
  return true;
}

template <typename LegalityCheck>
bool MoveGenerator<LegalityCheck>::
generateSlide(const Point& p, const Point& dir, MoveCallback& callback) const {
  Point q = p + dir;
  while (m_state.board().valid(q)) {
    if (!addMove(Move(p, q)))
      return false;
    q += dir;
  }
  
  return true;
}


template <typename LegalityCheck>
bool MoveGenerator<LegalityCheck>::addMove(const Move& m, MoveCallback& callback) const {
  LegalityCheck check(m_state);
  if (check.legal(m)) {
    return callback(m);
  }
  
  return true;
}

template <typename LegalityCheck>
bool MoveGenerator<LegalityCheck>::addAllPromotions(const Move& m, MoveCallback& callback) const {
  return addMove(Move(m.from(), m.to(), Piece::QUEEN), callback) &&
  addMove(Move(m.from(), m.to(), Piece::ROOK), callback) &&
  addMove(Move(m.from(), m.to(), Piece::KNIGHT), callback) &&
  addMove(Move(m.from(), m.to(), Piece::BISHOP), callback);
}



} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__MOVEGENERATOR_H

