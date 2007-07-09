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


typedef UnwrappedGraphicalAPI<ChessVariant> ChessGraphicalAPI;

class ChessAnimator {
  ChessGraphicalAPI::Ptr m_cinterface;
  Random m_random;

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
        NamedSprite sprite = m_cinterface->setPiece(i, f, /*false,*/ false);
        res->addPreAnimation( DropAnimationPtr(new DropAnimation(sprite.sprite())) );
      }
      else if (c && !f) {
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        res->addPreAnimation( CaptureAnimationPtr(new CaptureAnimation(old_sprite.sprite())) );
      }
      else if(c && f && !(c == f) ) {
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        NamedSprite sprite = m_cinterface->setPiece(i, f, /*false,*/ false);
        res->addPreAnimation( PromotionAnimationPtr(new PromotionAnimation(old_sprite.sprite(), sprite.sprite())) );
      }
    }

    //BROKEN: implement pool update


    return res;
  }

  boost::shared_ptr<AnimationGroup> forward(const ChessPosition& final, const ChessMove& move) {
    AnimationGroupPtr res(new AnimationGroup);
    MovementAnimationPtr ma;

    NamedSprite piece = m_cinterface->takeSprite(move.from);
    NamedSprite captured = m_cinterface->takeSprite(move.to);
    m_cinterface->setSprite(move.to, piece);

    if (piece)
      res->addPreAnimation(ma = MovementAnimationPtr(new MovementAnimation(piece.sprite(),
                                              m_cinterface->converter()->toReal(move.to))));
    else
      std::cout << "Bug!!!!" << std::endl;
    if (captured)
      res->addPostAnimation(ExplodeAnimationPtr(new ExplodeAnimation(captured.sprite(), m_random)));

    if (move.type() == ChessMove::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);
      NamedSprite capturedPawn = m_cinterface->takeSprite(phantom);

      if (capturedPawn) {
        QPoint real = m_cinterface->converter()->toReal(phantom);
        res->addPostAnimation(FadeAnimationPtr(new FadeAnimation(capturedPawn.sprite(),
                                                                          real, 255, 0)));
      }
      else
        std::cout << "Bug!!!!" << std::endl;
    }
    else if (move.type() == ChessMove::Promotion) {
      ChessPiece promoted = final.get(move.to);

      if (promoted) {
        QPoint real = m_cinterface->converter()->toReal(move.to);
        NamedSprite old_sprite = m_cinterface->getSprite(move.to);
        NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, /*false,*/ false);

        if (ma)
          ma->setTarget(new_sprite.sprite());

        res->addPostAnimation( FadeAnimationPtr(new FadeAnimation(old_sprite.sprite(), real, 255, 0)) );
        res->addPostAnimation( FadeAnimationPtr(new FadeAnimation(new_sprite.sprite(), real, 0, 255)) );
      }
      else
        std::cout << "Bug!!!!" << std::endl;
    }
    else if (move.type() == ChessMove::KingSideCastling) {
      Point rookSquare = move.to + Point(1,0);
      Point rookDestination = move.from + Point(1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookSquare);
      m_cinterface->setSprite(rookDestination, rook);

      res->addPreAnimation(
        shared_ptr<Animation>(
          new MovementAnimation(rook.sprite(),
            m_cinterface->converter()->toReal(rookDestination)))
      );
    }
    else if (move.type() == ChessMove::QueenSideCastling) {
      Point rookSquare = move.to + Point(-2,0);
      Point rookDestination = move.from + Point(-1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookSquare);
      m_cinterface->setSprite(rookDestination, rook);

      res->addPreAnimation(
        shared_ptr<Animation>(
          new MovementAnimation(rook.sprite(),
            m_cinterface->converter()->toReal(rookDestination)))
      );
    }

    return res;
  }

  boost::shared_ptr<AnimationGroup> back(const ChessPosition& final, const ChessMove& move) {
    AnimationGroupPtr res(new AnimationGroup);
    MovementAnimationPtr ma;

    NamedSprite piece = m_cinterface->takeSprite(move.to);
    NamedSprite captured;
    if (ChessPiece captured_piece = final.get(move.to)) {
      captured = m_cinterface->setPiece(move.to, captured_piece, /*false,*/ false);
      res->addPreAnimation(FadeAnimationPtr(new FadeAnimation(captured.sprite(),
                                                m_cinterface->converter()->toReal(move.to), 0, 255)));
    }

    if (!piece) {
      piece = m_cinterface->setPiece(move.to, final.get(move.from), /*false,*/ false);
      res->addPreAnimation(FadeAnimationPtr(new FadeAnimation(piece.sprite(),
                                              m_cinterface->converter()->toReal(move.to), 0, 255)));
    }

    m_cinterface->setSprite(move.from, piece);


    if (move.type() == ChessMove::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);

      if (ChessPiece pawn_piece = final.get(phantom)) {
        NamedSprite capturedPawn = m_cinterface->setPiece(phantom, pawn_piece, /*false,*/ false);
        res->addPreAnimation(FadeAnimationPtr(new FadeAnimation(capturedPawn.sprite(),
                                                    m_cinterface->converter()->toReal(phantom),
                                                    0, 255)));
      }
    }
    else if (move.type() == ChessMove::Promotion) {
      ChessPiece pawn_piece = final.get(move.from);
      if (pawn_piece) {
        NamedSprite pawn = m_cinterface->setPiece(move.from, pawn_piece, /*false,*/ false);
        res->addPreAnimation(FadeAnimationPtr(new FadeAnimation(pawn.sprite(),
                                 m_cinterface->converter()->toReal(move.to), 0, 255)));
        res->addPreAnimation(FadeAnimationPtr(new FadeAnimation(piece.sprite(),
                                 m_cinterface->converter()->toReal(move.to), 255, 0)));

        piece = pawn;
      }
    }
    else if (move.type() == ChessMove::KingSideCastling) {
      Point rookSquare = move.to + Point(1,0);
      Point rookDestination = move.from + Point(1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookDestination);
      m_cinterface->setSprite(rookSquare, rook);

      res->addPreAnimation(
        shared_ptr<Animation>(
          new MovementAnimation(rook.sprite(),
            m_cinterface->converter()->toReal(rookSquare)))
      );
    }
    else if (move.type() == ChessMove::QueenSideCastling) {
      Point rookSquare = move.to + Point(-2,0);
      Point rookDestination = move.from + Point(-1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookDestination);
      m_cinterface->setSprite(rookSquare, rook);

      res->addPreAnimation(
        shared_ptr<Animation>(
          new MovementAnimation(rook.sprite(),
            m_cinterface->converter()->toReal(rookSquare)))
      );
    }

    res->addPreAnimation(ma = MovementAnimationPtr(new MovementAnimation(piece.sprite(),
                                              m_cinterface->converter()->toReal(move.from))));
    return res;
  }
};

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
