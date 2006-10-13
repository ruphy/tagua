/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <memory>

#include "variants/xchess/animator.h"
#include "variants/xchess/move.h"
#include "animation.h"
#include "graphicalposition.h"
#include "piecesprite.h"
#include "kboard.h"
#include "pointconverter.h"
#include "position.h"

using namespace boost;

ChessAnimator::ChessAnimator(PointConverter* converter, GraphicalPosition* position)
: m_converter(converter)
, m_position(position)
, m_anim_movement(false)
, m_anim_explode(false)
, m_anim_fade(false)
, m_anim_rotate(false) {
  if(position->getBoolSetting("animations", true)) {
    m_anim_movement = (bool)position->getBoolSetting("animations.movement", true);
    m_anim_explode = (bool)position->getBoolSetting("animations.explode", true);
    m_anim_fade = (bool)position->getBoolSetting("animations.fading", true);
    m_anim_rotate = (bool)position->getBoolSetting("animations.transform", true);
  }
}

shared_ptr<MovementAnimation> ChessAnimator::createMovementAnimation(
                                const Element& element,
                                const QPoint& destination) {
  if (element.piece()->type() == static_cast<int>(KNIGHT))
    return shared_ptr<MovementAnimation>(new KnightMovementAnimation(element.sprite(),
                                                       destination, m_anim_rotate, 1.0));
  else
    return shared_ptr<MovementAnimation>(new MovementAnimation(element.sprite(),
                                                                destination, 1.0));
}

ChessAnimator::AnimationPtr ChessAnimator::warp(AbstractPosition::Ptr final) {
  //std::cout << "Chess warp!!!" << std::endl;

  AnimationPtr res(new AnimationGroup);
  for (Point i = m_position->first(); i <= m_position->last(); i = m_position->next(i)) {
    QPoint real = m_converter->toReal(i);
    Element p = m_position->getElement(i);
    AbstractPiece::Ptr q = final->get(i);
    shared_ptr<Animation> a;

    if (p) {
      shared_ptr<PieceSprite> sprite = p.sprite();
      Q_ASSERT(sprite);

      if (!p.piece()->equals(q)) {
        shared_ptr<PieceSprite> sprite = p.sprite();

        if (q) {
          a = shared_ptr<Animation>(new PromotionAnimation( sprite,
                                          m_position->setPiece(i, q, false, false) ));
        }
        else {
          // remove it
          m_position->removeElement(i);
          a = shared_ptr<Animation>(new CaptureAnimation(sprite));
        }
      }
    }
    else if (q) {
      a = shared_ptr<Animation>(new DropAnimation( m_position->setPiece(i, q, false, false) ));
    }

    if (a) res->addPreAnimation(a);
  }

  return res;
}

shared_ptr<Animation> ChessAnimator::createCapture(const Point& p,
                                                         const Element& /*piece*/,
                                                         const Element& captured,
                                                         AbstractPosition::Ptr /*pos*/) {
  if (!captured)
    return shared_ptr<Animation>();
  else if(m_anim_explode)
    return shared_ptr<Animation>(new ExplodeAnimation(captured.sprite(), m_random));
  else if(m_anim_fade)
    return shared_ptr<Animation>(new FadeAnimation(captured.sprite(),
                                              m_converter->toReal(p), 255, 0));
  else
    return shared_ptr<Animation>(new CaptureAnimation(captured.sprite()));
}

ChessAnimator::AnimationPtr ChessAnimator::forward(AbstractPosition::Ptr final,
                                                   const ChessMove& move) {
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
  ConflictingAnimation* conflict;
  QPoint hotSpot = piece.sprite()->pos();
  hotSpot += QPoint(piece.sprite()->pixmap().width() / 2, piece.sprite()->pixmap().height() / 2);

  if(!m_anim_movement) {
    InstantAnimation* a = new InstantAnimation(piece.sprite(), m_converter->toReal(move.to));
    conflict = a;
    mainAnimation = shared_ptr<InstantAnimation>(a);
  }
  else if (m_converter->toLogical(hotSpot) == move.to) {
    MovementAnimation* a = new MovementAnimation(piece.sprite(), m_converter->toReal(move.to));
    conflict = a;
    mainAnimation = shared_ptr<MovementAnimation>(a);
  }
  else {
    shared_ptr<MovementAnimation> a = createMovementAnimation(piece, m_converter->toReal(move.to));
    conflict = a.get();
    mainAnimation = a;
  }
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
      shared_ptr<PieceSprite> promoted_sprite = m_position->setPiece(move.to, promoted);

      conflict->setTarget(promoted_sprite);

      if(m_anim_fade) {
        res->addPostAnimation(
          shared_ptr<Animation>(new FadeAnimation(piece.sprite(), real, 255, 0))
        );
        res->addPostAnimation(
          shared_ptr<Animation>(new FadeAnimation(promoted_sprite, real, 0, 255))
        );
      }
      else
        res->addPostAnimation(shared_ptr<Animation>(new PromotionAnimation(piece.sprite(), promoted_sprite)));
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
}

ChessAnimator::AnimationPtr ChessAnimator::back(AbstractPosition::Ptr final,
                                                const ChessMove& move) {
  bool add_movement_animation = true;
  AnimationPtr res(new AnimationGroup);
  AbstractPiece::Ptr captured = final->get(move.to);
  Element piece = m_position->getElement(move.to);

  // the piece could have disappeared
  // make it reappear!
  if (!piece) {
    Q_ASSERT(final->get(move.from));
    AbstractPiece::Ptr disappeared = final->get(move.from);
    shared_ptr<PieceSprite> sprite = m_position->setPiece(move.to, disappeared, false, false);
    res->addPreAnimation(shared_ptr<Animation>(new DropAnimation(sprite)));
    piece = Element(disappeared, sprite);
  }

  m_position->setElement(move.from, piece);

  if (captured) {
    // recreate captured piece sprite
    QPoint real = m_converter->toReal(move.to);
    if(m_anim_fade)
      res->addPreAnimation( shared_ptr<Animation>(new FadeAnimation(
              m_position->setPiece(move.to, captured, false, false), real, 0, 255 ) ));
    else
      res->addPreAnimation( shared_ptr<Animation>(new DropAnimation(
              m_position->setPiece(move.to, captured, false, false) ) ));
  }
  else
    m_position->removeElement(move.to);

  if (move.type() == ChessMove::EnPassantCapture) {
    Point phantom(move.to.x, move.from.y);
    AbstractPiece::Ptr capturedPawn = final->get(phantom);
    if (capturedPawn)
      res->addPreAnimation( shared_ptr<Animation>(new DropAnimation(
                m_position->setPiece( phantom, capturedPawn, false, false) ) ));
  }

  else if (move.type() == ChessMove::Promotion) {
    AbstractPiece::Ptr promotedPawn = final->get(move.from);

    shared_ptr<PieceSprite> pawn = m_position->setPiece(move.from, promotedPawn, false, false);
    pawn->moveTo(m_converter->toReal(move.to));

    // 'piece' is the promoted piece
    res->addPreAnimation(shared_ptr<Animation>(
      new PromotionAnimation(piece.sprite(), pawn)));

    // add movement animation
    add_movement_animation = false;
    shared_ptr<Animation> movementAnimation;
    {
      ConflictingAnimation* conflict;
      if (m_anim_movement) {
        shared_ptr<MovementAnimation> a(new MovementAnimation(pawn, m_converter->toReal(move.from)));
        conflict = a.get();
        movementAnimation = a;
      }
      else {
        shared_ptr<InstantAnimation> a(
          new InstantAnimation(pawn, m_converter->toReal(move.from)));
        conflict = a.get();
        movementAnimation = a;
      }

      conflict->setSource(piece.sprite());
    }
    res->addPreAnimation(movementAnimation);
  }

  else if (move.type() == ChessMove::KingSideCastling) {
    Point rookSquare = move.to + Point(1,0);
    Point rookDestination = move.from + Point(1,0);

    Element rook = m_position->getElement(rookDestination);
    Q_ASSERT(rook);

    res->addPreAnimation(
      shared_ptr<Animation>(
        m_anim_movement
        ? (Animation*)new MovementAnimation(rook.sprite(), m_converter->toReal(rookSquare))
        : (Animation*)new InstantAnimation(rook.sprite(), m_converter->toReal(rookSquare))
      )
    );
    m_position->setElement(rookSquare, rook);
    m_position->removeElement(rookDestination);

  }

  else if (move.type() == ChessMove::QueenSideCastling) {
    Point rookSquare = move.to - Point(2,0);
    Point rookDestination = move.from - Point(1,0);

    Element rook = m_position->getElement(rookDestination);
    Q_ASSERT(rook);

    res->addPreAnimation(
      shared_ptr<Animation>(
        m_anim_movement
        ? (Animation*)new MovementAnimation(rook.sprite(), m_converter->toReal(rookSquare))
        : (Animation*)new InstantAnimation(rook.sprite(), m_converter->toReal(rookSquare))
      )
    );

    m_position->setElement(rookSquare, rook);
    m_position->removeElement(rookDestination);
  }

  if (add_movement_animation) {
    res->addPreAnimation(
      m_anim_movement
      ? shared_ptr<Animation>(createMovementAnimation(piece, m_converter->toReal(move.from)))
      : shared_ptr<Animation>(new InstantAnimation(piece.sprite(), m_converter->toReal(move.from)))
    );
  }

  finalizeBackAnimation(res, final, move);

  AnimationPtr warpingAnimation(new AnimationGroup);
  warpingAnimation->addPreAnimation(res);
  warpingAnimation->addPostAnimation(warp(final));

  return warpingAnimation;
}


