/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "animator.h"
#include "animationfactory.h"

template <typename Variant>
AnimationGroupPtr SimpleAnimator<Variant>::warp(const Position& final) {
  const ChessPosition* current = m_cinterface->position();
  AnimationFactory res(m_cinterface->inner());
  
  for (Point i = current->first(); i <= current->last(); i = current->next(i)) {
    ChessPiece c = current->get(i);
    ChessPiece f = final.get(i);
  
    if( !c && f ) {
      //current->set(i, f);
      NamedSprite sprite = m_cinterface->setPiece(i, f, false);
      res.addPreAnimation(Animate::appear(sprite), Animate::Instant);
    }
    else if (c && !f) {
      NamedSprite old_sprite = m_cinterface->takeSprite(i);
      res.addPreAnimation(Animate::disappear(old_sprite), Animate::Instant);
    }
    else if(c && f && !(c == f) ) {
      NamedSprite old_sprite = m_cinterface->takeSprite(i);
      NamedSprite sprite = m_cinterface->setPiece(i, f, false);
      res.addPreAnimation(Animate::morph(old_sprite, sprite), Animate::Instant);
    }
  }
  
  return res;
}

template <typename Variant>
SchemePtr SimpleAnimator<Variant>::movement(const NamedSprite& sprite, const Point& from, const Point& to) {
  bool knight = m_cinterface->position()->get(from).type() == KNIGHT;
  int mtype = knight
    ? Animate::move::LShaped | Animate::move::Rotating 
    : Animate::move::Straight;
  return SchemePtr(new Animate::move(sprite, to, mtype));
}

template <typename Variant>
AnimationGroupPtr SimpleAnimator<Variant>::forward(const Position& final, const Move& move) {
  AnimationFactory res(m_cinterface->inner());

  NamedSprite piece = m_cinterface->takeSprite(move.from);
  NamedSprite captured = m_cinterface->takeSprite(move.to);
  m_cinterface->setSprite(move.to, piece);

  if (piece)
    res.addPreAnimation(*movement(piece, move.from, move.to));
  else
    ERROR("Bug!!!");

  if (captured)
    res.addPostAnimation(Animate::destroy(captured));

  if (move.type() == Move::EnPassantCapture) {
    Point phantom(move.to.x, move.from.y);
    NamedSprite capturedPawn = m_cinterface->takeSprite(phantom);

    if (capturedPawn) {
      QPoint real = m_cinterface->converter()->toReal(phantom);
      res.addPostAnimation(Animate::disappear(capturedPawn));
    }
    else
      ERROR("Bug!!!");
  }
  else if (move.type() == Move::Promotion) {
    Piece promoted = final.get(move.to);

    if (promoted) {
      QPoint real = m_cinterface->converter()->toReal(move.to);
      NamedSprite old_sprite = m_cinterface->getSprite(move.to);
      NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, /*false,*/ false);

      res.addPostAnimation(Animate::morph(old_sprite, new_sprite));
    }
    else
      ERROR("Bug!!!");
  }
  else if (move.type() == Move::KingSideCastling) {
    Point rookSquare = move.to + Point(1,0);
    Point rookDestination = move.from + Point(1,0);

    NamedSprite rook = m_cinterface->takeSprite(rookSquare);
    m_cinterface->setSprite(rookDestination, rook);
    res.addPreAnimation(Animate::move(rook, rookDestination));
  }
  else if (move.type() == Move::QueenSideCastling) {
    Point rookSquare = move.to + Point(-2,0);
    Point rookDestination = move.from + Point(-1,0);

    NamedSprite rook = m_cinterface->takeSprite(rookSquare);
    m_cinterface->setSprite(rookDestination, rook);
    res.addPreAnimation(Animate::move(rook, rookDestination));
  }

  return res;
}

template <typename Variant>
AnimationGroupPtr SimpleAnimator<Variant>::back(const Position& final, const Move& move) {
  AnimationFactory res(m_cinterface->inner());

  NamedSprite piece = m_cinterface->takeSprite(move.to);
  NamedSprite captured;
  if (Piece captured_piece = final.get(move.to)) {
    captured = m_cinterface->setPiece(move.to, captured_piece, false);
    res.addPreAnimation(Animate::appear(captured));
  }

  if (!piece) {
    piece = m_cinterface->createPiece(move.to, final.get(move.from), false);
    res.addPreAnimation(Animate::appear(piece));
  }

  m_cinterface->setSprite(move.from, piece);


  if (move.type() == Move::EnPassantCapture) {
    Point phantom(move.to.x, move.from.y);

    if (Piece pawn_piece = final.get(phantom)) {
      NamedSprite captured_pawn = m_cinterface->setPiece(phantom, pawn_piece, false);
      res.addPreAnimation(Animate::appear(captured_pawn));
    }
  }
  else if (move.type() == Move::Promotion) {
    Piece pawn_piece = final.get(move.from);
    if (pawn_piece) {
      NamedSprite pawn = m_cinterface->createPiece(move.to, pawn_piece, false);
      res.addPreAnimation(Animate::morph(piece, pawn));

      // replace piece with pawn
      m_cinterface->setSprite(move.from, pawn);
      piece = pawn;
    }
  }
  else if (move.type() == Move::KingSideCastling) {
    Point rookSquare = move.to + Point(1,0);
    Point rookDestination = move.from + Point(1,0);

    NamedSprite rook = m_cinterface->takeSprite(rookDestination);
    m_cinterface->setSprite(rookSquare, rook);

    res.addPreAnimation(Animate::move(rook, rookSquare));
  }
  else if (move.type() == Move::QueenSideCastling) {
    Point rookSquare = move.to + Point(-2,0);
    Point rookDestination = move.from + Point(-1,0);

    NamedSprite rook = m_cinterface->takeSprite(rookDestination);
    m_cinterface->setSprite(rookSquare, rook);

    res.addPreAnimation(Animate::move(rook, rookSquare));
  }
  
  res.addPreAnimation(*movement(piece, move.to, move.from));
  
  return res;

}

template <typename Variant>
void SimpleAnimator<Variant>::updatePool(const Position& final) {
  final.dump();

  for(int color = 0; color < 2; color++) {
    typename Piece::Color c = static_cast<typename Piece::Color>(color);
    const typename Position::PlayerPool& before = m_cinterface->position()->rawPool().find(c)->second;
    const typename Position::PlayerPool& after = final.rawPool().find(c)->second;
    typename Position::PlayerPool::const_iterator before_it = before.begin();
    typename Position::PlayerPool::const_iterator after_it = after.begin();

    int pos = 0;

    // oh, a nice bunch of write-only magic shit
    while(before_it != before.end() || after_it != after.end()) {
      if(after_it == after.end() || (before_it != before.end()
              && before_it->first < after_it->first )) {
        for(int i=0;i<before_it->second;i++)
          m_cinterface->takePoolSprite(color, pos);
        ++before_it;
      }
      else if (before_it == before.end() || (after_it != after.end()
              && after_it->first < before_it->first )) {
        for(int i=0;i<after_it->second;i++)
          m_cinterface->insertPoolPiece(color, pos, Piece(c, after_it->first) );
        pos += after_it->second;
        ++after_it;
      }
      else {
        Q_ASSERT(after_it->first == before_it->first);
        if(before_it->second < after_it->second)
        for(int i=0;i<after_it->second - before_it->second;i++)
          m_cinterface->insertPoolPiece(color, pos, Piece(c, after_it->first) );
        else if(before_it->second > after_it->second)
        for(int i=0;i<before_it->second - after_it->second;i++)
          m_cinterface->takePoolSprite(color, pos);
        pos += after_it->second;
        ++after_it;
        ++before_it;
      }
    }
  }
}

