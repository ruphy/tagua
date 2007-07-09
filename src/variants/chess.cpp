/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QPainter>
#include <boost/shared_ptr.hpp>
#include "variants/chess.h"
#include "common.h"
#include "kboard_wrapped.h"
#include "moveserializer.impl.h"
#include "xchess/animator.impl.h"
#include "piecefunction.h"
#include "animation.h"

using namespace boost;
typedef boost::shared_ptr<class Animation> AnimationPtr;

const char *ChessVariant::m_name = "Chess";
const char *ChessVariant::m_theme_proxy = "Chess";
VariantInfo* ChessVariant::static_chess_variant = 0;



//BEGIN Dream code

typedef UnwrappedGraphicalAPI<ChessVariant> ChessGraphicalAPI;

class ChessAnimator {
  ChessGraphicalAPI::Ptr m_cinterface;
public:
  ChessAnimator(ChessGraphicalAPI::Ptr cinterface)
    : m_cinterface(cinterface) {
  }

  AnimationGroupPtr warp(const ChessPosition& final) {
    const ChessPosition* current = m_cinterface->position();
    AnimationGroupPtr res(new AnimationGroup);

    for (Point i = current->first(); i <= current->last(); i = current->next(i)) {
      ChessPiece c = current->get(i);
      ChessPiece f = final.get(i);

      if( !c && f ) {
        //current->set(i, f);
        NamedSprite sprite = m_cinterface->setPiece(i, f, false, false);
        res->addPreAnimation(m_cinterface->appearAnimation(sprite, GraphicalAPI::Instant));
      }
      else if (c && !f) {
        //current->set(i, NULL);
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        res->addPreAnimation(m_cinterface->disappearAnimation(old_sprite, GraphicalAPI::Instant));
      }
      else if(c && f && !(c == f) ) {
        //current->set(i, f);
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        NamedSprite sprite = m_cinterface->setPiece(i, f, false, false);
        res->addPreAnimation(m_cinterface->morphAnimation(old_sprite, sprite, GraphicalAPI::Instant));
      }
    }

    //BROKEN: implement pool update


    return res;
  }

  boost::shared_ptr<AnimationGroup> forward(const ChessPosition& final, const ChessMove& move) {
    AnimationGroupPtr res(new AnimationGroup);
    //ChessPiece piece = current->get(move.from);

    NamedSprite piece = m_cinterface->takeSprite(move.from);
    NamedSprite captured = m_cinterface->takeSprite(move.to);
    m_cinterface->setSprite(move.to, piece);

    if (piece)
      res->addPreAnimation(m_cinterface->moveAnimation(piece, move.to));
    else
      std::cout << "Bug!!!!" << std::endl;
    if (captured)
      res->addPostAnimation(m_cinterface->destroyAnimation(captured));

    if (move.type() == ChessMove::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);
      NamedSprite capturedPawn = m_cinterface->takeSprite(phantom);

      if (capturedPawn) {
        QPoint real = m_cinterface->converter()->toReal(phantom);
        res->addPostAnimation(m_cinterface->disappearAnimation(capturedPawn));
      }
      else
        std::cout << "Bug!!!!" << std::endl;
    }
    else if (move.type() == ChessMove::Promotion) {
      ChessPiece promoted = final.get(move.to);

      if (promoted) {
        QPoint real = m_cinterface->converter()->toReal(move.to);
        NamedSprite old_sprite = m_cinterface->getSprite(move.to);
        NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, false, false);

				res->addPostAnimation(m_cinterface->morphAnimation(old_sprite, new_sprite));
      }
      else
        std::cout << "Bug!!!!" << std::endl;
    }
    else if (move.type() == ChessMove::KingSideCastling) {
      Point rookSquare = move.to + Point(1,0);
      Point rookDestination = move.from + Point(1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookSquare);
      m_cinterface->setSprite(rookDestination, rook);
      res->addPreAnimation(m_cinterface->moveAnimation(rook, rookDestination));
    }
    else if (move.type() == ChessMove::QueenSideCastling) {
      Point rookSquare = move.to + Point(-2,0);
      Point rookDestination = move.from + Point(-1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookSquare);
      m_cinterface->setSprite(rookDestination, rook);
      res->addPreAnimation(m_cinterface->moveAnimation(rook, rookDestination));
    }

    return res;
  }

  boost::shared_ptr<AnimationGroup> back(const ChessPosition& final, const ChessMove& move) {
    AnimationGroupPtr res(new AnimationGroup);

    NamedSprite piece = m_cinterface->takeSprite(move.to);
    NamedSprite captured;
    if (ChessPiece captured_piece = final.get(move.to)) {
      captured = m_cinterface->setPiece(move.to, captured_piece, false, false);
      res->addPreAnimation(m_cinterface->appearAnimation(captured));
    }

    if (!piece) {
      piece = m_cinterface->createPiece(move.to, final.get(move.from), false, false);
      res->addPreAnimation(m_cinterface->appearAnimation(piece));
      res->addPreAnimation(FadeAnimationPtr(new FadeAnimation(piece.sprite(),
                                              m_cinterface->converter()->toReal(move.to), 0, 255)));
    }

    m_cinterface->setSprite(move.from, piece);


    if (move.type() == ChessMove::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);

      if (ChessPiece pawn_piece = final.get(phantom)) {
        NamedSprite captured_pawn = m_cinterface->setPiece(phantom, pawn_piece, false, false);
        res->addPreAnimation(m_cinterface->appearAnimation(captured_pawn));
      }
    }
    else if (move.type() == ChessMove::Promotion) {
      ChessPiece pawn_piece = final.get(move.from);
      if (pawn_piece) {
        NamedSprite pawn = m_cinterface->createPiece(move.to, pawn_piece, false, false);
        res->addPreAnimation(m_cinterface->morphAnimation(piece, pawn));

				// replace piece with pawn
        m_cinterface->setSprite(move.from, pawn);
        piece = pawn;
      }
    }
    else if (move.type() == ChessMove::KingSideCastling) {
      Point rookSquare = move.to + Point(1,0);
      Point rookDestination = move.from + Point(1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookDestination);
      m_cinterface->setSprite(rookSquare, rook);

      res->addPreAnimation(m_cinterface->moveAnimation(rook, rookSquare));
    }
    else if (move.type() == ChessMove::QueenSideCastling) {
      Point rookSquare = move.to + Point(-2,0);
      Point rookDestination = move.from + Point(-1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookDestination);
      m_cinterface->setSprite(rookSquare, rook);

			res->addPreAnimation(m_cinterface->moveAnimation(rook, rookSquare));
    }

    res->addPreAnimation(m_cinterface->moveAnimation(piece, move.from));
    return res;
  }
};


//END Dream code


void ChessVariant::forallPieces(PieceFunction& f) {
  f(WHITE, KING);
  f(WHITE, QUEEN);
  f(WHITE, ROOK);
  f(WHITE, BISHOP);
  f(WHITE, KNIGHT);
  f(WHITE, PAWN);
  f(BLACK, KING);
  f(BLACK, QUEEN);
  f(BLACK, ROOK);
  f(BLACK, BISHOP);
  f(BLACK, KNIGHT);
  f(BLACK, PAWN);
}

VariantInfo* ChessVariant::info() {
  if (!static_chess_variant)
    static_chess_variant = new WrappedVariantInfo<ChessVariant>;
  return static_chess_variant;
}
