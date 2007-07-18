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
#include "tagua_wrapped.h"
#include "moveserializer.impl.h"
#include "xchess/animator.impl.h"
#include "piecefunction.h"
#include "animation.h"

using namespace boost;
typedef boost::shared_ptr<class Animation> AnimationPtr;

const char *ChessVariant::m_name = "Chess";
const char *ChessVariant::m_theme_proxy = "Chess";
VariantInfo* ChessVariant::static_chess_variant = 0;

#if 0
class ChessAnimator {
  ChessGraphicalAPI::Ptr m_cinterface;
public:
  ChessAnimator(ChessGraphicalAPI::Ptr cinterface)
    : m_cinterface(cinterface) {
  }

  AnimationGroupPtr warp(const ChessPosition& final) {
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

    //BROKEN: implement pool update


    return res;
  }

  boost::shared_ptr<AnimationGroup> forward(const ChessPosition& final, const ChessMove& move) {
    AnimationFactory res(m_cinterface->inner());

    NamedSprite piece = m_cinterface->takeSprite(move.from);
    NamedSprite captured = m_cinterface->takeSprite(move.to);
    m_cinterface->setSprite(move.to, piece);

    if (piece) {
      bool knight = m_cinterface->position()->get(move.from).type() == KNIGHT;
      int mtype = knight
        ? Animate::move::LShaped | Animate::move::Rotating
        : Animate::move::Straight;
      res.addPreAnimation(Animate::move(piece, move.to, mtype));
    }
    else
      ERROR("Bug!!!");

    if (captured)
      res.addPostAnimation(Animate::destroy(captured));

    if (move.type() == ChessMove::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);
      NamedSprite capturedPawn = m_cinterface->takeSprite(phantom);

      if (capturedPawn) {
        QPoint real = m_cinterface->converter()->toReal(phantom);
        res.addPostAnimation(Animate::disappear(capturedPawn));
      }
      else
        ERROR("Bug!!!");
    }
    else if (move.type() == ChessMove::Promotion) {
      ChessPiece promoted = final.get(move.to);

      if (promoted) {
        QPoint real = m_cinterface->converter()->toReal(move.to);
        NamedSprite old_sprite = m_cinterface->getSprite(move.to);
        NamedSprite new_sprite = m_cinterface->setPiece(move.to, promoted, /*false,*/ false);

        res.addPostAnimation(Animate::morph(old_sprite, new_sprite));
      }
      else
        ERROR("Bug!!!");
    }
    else if (move.type() == ChessMove::KingSideCastling) {
      Point rookSquare = move.to + Point(1,0);
      Point rookDestination = move.from + Point(1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookSquare);
      m_cinterface->setSprite(rookDestination, rook);
      res.addPreAnimation(Animate::move(rook, rookDestination));
    }
    else if (move.type() == ChessMove::QueenSideCastling) {
      Point rookSquare = move.to + Point(-2,0);
      Point rookDestination = move.from + Point(-1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookSquare);
      m_cinterface->setSprite(rookDestination, rook);
      res.addPreAnimation(Animate::move(rook, rookDestination));
    }

    return res;
  }

  boost::shared_ptr<AnimationGroup> back(const ChessPosition& final, const ChessMove& move) {
    AnimationFactory res(m_cinterface->inner());

    NamedSprite piece = m_cinterface->takeSprite(move.to);
    NamedSprite captured;
    if (ChessPiece captured_piece = final.get(move.to)) {
      captured = m_cinterface->setPiece(move.to, captured_piece, false);
      res.addPreAnimation(Animate::appear(captured));
    }

    if (!piece) {
      piece = m_cinterface->createPiece(move.to, final.get(move.from), false);
      res.addPreAnimation(Animate::appear(piece));
    }

    m_cinterface->setSprite(move.from, piece);


    if (move.type() == ChessMove::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);

      if (ChessPiece pawn_piece = final.get(phantom)) {
        NamedSprite captured_pawn = m_cinterface->setPiece(phantom, pawn_piece, false);
        res.addPreAnimation(Animate::appear(captured_pawn));
      }
    }
    else if (move.type() == ChessMove::Promotion) {
      ChessPiece pawn_piece = final.get(move.from);
      if (pawn_piece) {
        NamedSprite pawn = m_cinterface->createPiece(move.to, pawn_piece, false);
        res.addPreAnimation(Animate::morph(piece, pawn));
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

      res.addPreAnimation(Animate::move(rook, rookSquare));
    }
    else if (move.type() == ChessMove::QueenSideCastling) {
      Point rookSquare = move.to + Point(-2,0);
      Point rookDestination = move.from + Point(-1,0);

      NamedSprite rook = m_cinterface->takeSprite(rookDestination);
      m_cinterface->setSprite(rookSquare, rook);

      res.addPreAnimation(Animate::move(rook, rookSquare));
    }

    {
      bool knight = m_cinterface->position()->get(move.to).type() == KNIGHT;
      int mtype = knight
        ? Animate::move::LShaped | Animate::move::Rotating
        : Animate::move::Straight;
      res.addPreAnimation(Animate::move(piece, move.from, mtype));
    }
    return res;
  }
};
#endif

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

// piece factory
template <>
class PieceFactory<ChessVariant> {
public:
  static ChessPiece createPiece(const QString& description) {
    if (description.size() == 1) {
      QChar c = description[0];
      ChessPiece::Color color;

      if (c.category() == QChar::Letter_Uppercase) {
        color = WHITE;
      }
      else if (c.category() == QChar::Letter_Lowercase) {
        color = BLACK;
      }
      else {
        return ChessPiece();
      }

      ChessPiece::Type type = ChessPiece::getType(c);

      if (type != INVALID_TYPE) {
        return ChessPiece(color, type);
      }
    }

    return ChessPiece();
  }
};
