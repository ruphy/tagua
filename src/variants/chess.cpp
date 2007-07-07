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
        //current->set(i, f);
        NamedSprite sprite = m_cinterface->setPiece(i, f, false, false);
        res->addPreAnimation( DropAnimationPtr(new DropAnimation(sprite.sprite())) );
      }
      else if (c && !f) {
        //current->set(i, NULL);
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        res->addPreAnimation( CaptureAnimationPtr(new CaptureAnimation(old_sprite.sprite())) );
      }
      else if(c && f && !(c == f) ) {
        //current->set(i, f);
        NamedSprite old_sprite = m_cinterface->takeSprite(i);
        NamedSprite sprite = m_cinterface->setPiece(i, f, false, false);
        res->addPreAnimation( PromotionAnimationPtr(new PromotionAnimation(old_sprite.sprite(), sprite.sprite())) );
      }
    }

    //BROKEN: implement pool update


    return res;
  }

  boost::shared_ptr<AnimationGroup> forward(const ChessPosition& final, const ChessMove& move) {
//     const ChessPosition* current = m_cinterface->position();

    AnimationGroupPtr res(new AnimationGroup);
    MovementAnimationPtr ma;
    //ChessPiece piece = current->get(move.from);

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
        NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, false, false);

        if(ma)
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

    #if (1-1) //OLD SHIT
    Element piece = m_position->getElement(move.from);

      Q_ASSERT(m_position->consistent());

      AnimationPtr res(new AnimationGroup);

      Element piece = m_position->getElement(move.from);
      Q_ASSERT(piece);
      Element captured = m_position->getElement(move.to);

      // update graphical position
      m_position->setElement(move.to, piece);
      Q_ASSERT(m_position->consistent());

      m_position->removeElement(move.from);
      Q_ASSERT(m_position->consistent());


      // add main animation
      // the animation must be direct when the piece is already
      // near the destination square
      shared_ptr<Animation> mainAnimation;
      QPoint hotSpot = piece.sprite()->pos();
      hotSpot += QPoint(piece.sprite()->pixmap().width() / 2, piece.sprite()->pixmap().height() / 2);

      if(!m_anim_movement)
        mainAnimation = shared_ptr<InstantAnimation>(
          new InstantAnimation(piece.sprite(), m_converter->toReal(move.to)));
      else if (m_converter->toLogical(hotSpot) == move.to)
        mainAnimation = shared_ptr<MovementAnimation>(
          new MovementAnimation(piece.sprite(), m_converter->toReal(move.to)));
      else
        mainAnimation = createMovementAnimation(piece, m_converter->toReal(move.to));
      res->addPreAnimation(mainAnimation);

      res->addPostAnimation(createCapture(move.to, piece, captured, final));

      if (move.type() == ChessMove::EnPassantCapture) {
        Point phantom(move.to.x, move.from.y);
        Element capturedPawn = m_position->getElement(phantom);

        if (capturedPawn) {
          if(m_anim_fade)
            res->addPostAnimation(
              shared_ptr<Animation>(
                  new FadeAnimation(capturedPawn.sprite(),
                                    m_converter->toReal(phantom),
                                    255,0)));
          else
            res->addPostAnimation(
              shared_ptr<Animation>(
                  new CaptureAnimation(capturedPawn.sprite() )));
          m_position->removeElement(phantom);
        }
      }

      else if (move.type() == ChessMove::Promotion) {
        AbstractPiece::Ptr promoted = final->get(move.to);

        if (promoted) {
          QPoint real = m_converter->toReal(move.to);
          boost::shared_ptr<PieceSprite> pe_sprite = m_position->setPiece(move.to, promoted);

          if(MovementAnimation* mva = dynamic_cast<MovementAnimation*>(mainAnimation.get()))
            mva->setTarget(pe_sprite);

          if(m_anim_fade) {
            res->addPostAnimation(
              shared_ptr<Animation>(new FadeAnimation(piece.sprite(), real, 255, 0))
            );
            res->addPostAnimation(
              shared_ptr<Animation>(new FadeAnimation(pe_sprite, real, 0, 255))
            );
          }
          else {
            res->addPreAnimation(shared_ptr<Animation>(new CaptureAnimation(piece.sprite())));
            res->addPreAnimation(shared_ptr<Animation>(new DropAnimation(pe_sprite)));
          }
        }
      }

      else if (move.type() == ChessMove::KingSideCastling) {
        Point rookSquare = move.to + Point(1,0);
        Point rookDestination = move.from + Point(1,0);

        Element rook = m_position->getElement(rookSquare);
        Q_ASSERT(rook);
        m_position->setElement(rookDestination, rook);
        m_position->removeElement(rookSquare);

        res->addPreAnimation(
          shared_ptr<Animation>(
            m_anim_movement
            ? static_cast<Animation*>(new MovementAnimation(rook.sprite(), m_converter->toReal(rookDestination)))
            : static_cast<Animation*>(new InstantAnimation(rook.sprite(), m_converter->toReal(rookDestination)))
          )
        );

      }

      else if (move.type() == ChessMove::QueenSideCastling) {
        Point rookSquare = move.to - Point(2,0);
        Point rookDestination = move.from - Point(1,0);

        Element rook = m_position->getElement(rookSquare);
        Q_ASSERT(rook);
        m_position->setElement(rookDestination, rook);
        m_position->removeElement(rookSquare);

    //     if (m_anim_movement)
    //       res->addPreAnimation(
    //         shared_ptr<MovementAnimation>(new MovementAnimation(rook.sprite(), m_converter->toReal(rookDestination)))
    //       );
    //     else
    //       res->addPreAnimation(
    //         shared_ptr<Animation>(new InstantAnimation(rook.sprite(), m_converter->toReal(rookDestination)))
    //       );
        res->addPreAnimation(
          shared_ptr<Animation>(
            m_anim_movement
            ? static_cast<Animation*>(new MovementAnimation(rook.sprite(), m_converter->toReal(rookDestination)))
            : static_cast<Animation*>(new InstantAnimation(rook.sprite(), m_converter->toReal(rookDestination)))
          )
        );
      }

      Q_ASSERT(m_position->consistent());

      finalizeForwardAnimation(res, final, move);

      AnimationPtr warpingAnimation(new AnimationGroup);
      warpingAnimation->addPreAnimation(res);
      warpingAnimation->addPostAnimation(warp(final));

      return warpingAnimation;
    //BROKEN
    #endif
  }

  boost::shared_ptr<AnimationGroup> back(const ChessPosition& final, const ChessMove& /*move*/) {
    //BROKEN
    return warp(final);
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
