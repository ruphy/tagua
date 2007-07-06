/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GENERATOR_H
#define GENERATOR_H

#include <functional>
#include "variants/xchess/piecetype.h"
#include "point.h"

template <typename Pos>
class LegalMove {
  const Pos& m_pos;
  typedef typename Pos::Move Move;
public:
  bool operator()(Move& m) { return m_pos.testMove(m); }
  LegalMove(const Pos& pos) : m_pos(pos) { }
};

template <typename Pos>
class PseudolegalMove {
  const Pos& m_pos;
  typedef typename Pos::Move Move;
public:
  bool operator()(Move& m) { return m_pos.pseudolegal(m); }
  PseudolegalMove(const Pos& pos) : m_pos(pos) { }
};

template <typename Move>
class AbstractGenerator {
public:
  virtual ~AbstractGenerator(){};
  virtual std::vector<Move>& generate() = 0;
};

template <typename Pos, typename MoveTest>
class Generator : public AbstractGenerator<typename Pos::Move> {
public:
  virtual ~Generator(){};

  typedef typename Pos::Move Move;
  typedef typename Pos::Piece Piece;
  typedef typename Pos::Board Board;

  std::vector<Move>& generate();
  inline std::vector<Move>& moves() { return m_moves; }
  explicit Generator(const Pos&);
protected:
  const Pos& m_pos;
  std::vector<Move> m_moves;
  void generateFrom(const Point& p);
  MoveTest m_test;

  bool addMove(const Point& p, const Point& q);
  void generateSlide(const Point& p, const Point& dir);
  void generateBishop(const Point& p);
  void generateRook(const Point& p);
};

template <typename Pos, typename MoveTest>
class MoveGenerator : public Generator<Pos, MoveTest> {
public:
  virtual ~MoveGenerator(){};
  explicit MoveGenerator(const Pos& pos)
  : Generator<Pos, MoveTest>(pos) { }
};

//BEGIN Implementation

template <typename Pos, typename MoveTest>
Generator<Pos, MoveTest>::Generator(const Pos& pos)
: m_pos(pos)
, m_test(pos) { }

template <typename Pos, typename MoveTest>
std::vector<typename Pos::Move>& Generator<Pos, MoveTest>::generate() {
  for (Point from = m_pos.first(); from <= m_pos.last(); from = m_pos.next(from)) {
    generateFrom(from);
  }
  return m_moves;
}

template <typename Pos, typename MoveTest>
void Generator<Pos, MoveTest>::generateFrom(const Point& p) {
  Piece piece = m_pos[p];
  if (piece && piece.color() == m_pos.turn()) {
    switch (piece.type()) {
    case PAWN:
      {
        Point dir = Piece::direction(piece.color());
        addMove(p, p + dir);
        addMove(p, p + dir * 2);
        addMove(p, p + dir + Point(1, 0));
        addMove(p, p + dir - Point(1, 0));
      }
      break;
    case KNIGHT:
      addMove(p, p + Point(1,2));
      addMove(p, p + Point(1,-2));
      addMove(p, p + Point(-1,2));
      addMove(p, p + Point(-1,-2));
      addMove(p, p + Point(2,1));
      addMove(p, p + Point(2,-1));
      addMove(p, p + Point(-2,1));
      addMove(p, p + Point(-2,-1));
      break;
    case BISHOP:
      generateBishop(p);
      break;
    case ROOK:
      generateRook(p);
      break;
    case QUEEN:
      generateBishop(p);
      generateRook(p);
      break;
    case KING:
      addMove(p, p + Point(1,0));
      addMove(p, p + Point(1,1));
      addMove(p, p + Point(0,1));
      addMove(p, p + Point(-1,1));
      addMove(p, p + Point(-1,0));
      addMove(p, p + Point(-1,-1));
      addMove(p, p + Point(0,-1));
      addMove(p, p + Point(1,-1));
      break;
    default:
      break;
    }
  }
}

template <typename Pos, typename MoveTest>
void Generator<Pos, MoveTest>::generateSlide(const Point& p, const Point& dir) {
  Point q = p + dir;
  while (m_pos.valid(q) && addMove(p,q))
    q += dir;
}

template <typename Pos, typename MoveTest>
bool Generator<Pos, MoveTest>::addMove(const Point& p, const Point& q) {
  Move move(p,q);
  if (m_pos.valid(p) && m_pos.valid(q) && m_test(move)) {
    m_moves.push_back(move);
    return true;
  }
  else return false;
}

template <typename Pos, typename MoveTest>
void Generator<Pos, MoveTest>::generateBishop(const Point& p) {
  generateSlide(p, Point(1,1));
  generateSlide(p, Point(1,-1));
  generateSlide(p, Point(-1,1));
  generateSlide(p, Point(-1,-1));
}

template <typename Pos, typename MoveTest>
void Generator<Pos, MoveTest>::generateRook(const Point& p) {
  generateSlide(p, Point(1,0));
  generateSlide(p, Point(0,1));
  generateSlide(p, Point(-1,0));
  generateSlide(p, Point(0,-1));
}

//END Implementation

#endif
