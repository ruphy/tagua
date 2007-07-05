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
#include "highlevel.h"
#include "moveserializer.impl.h"
#include "xchess/animator.impl.h"
#include "piecefunction.h"
#include "unwrapped_graphicalapi.h"
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

  AnimationPtr warp(const ChessPosition* final) {
    const ChessPosition* current = m_cinterface->position();
    boost::shared_ptr<AnimationGroup> res(new AnimationGroup);

    for (Point i = current->first(); i <= current->last(); i = current->next(i)) {
      const ChessPiece* c = current->get(i);
      const ChessPiece* f = final->get(i);

      if( !c && f ) {
        //current->set(i, f);
        SpritePtr sprite = m_cinterface->setSprite(i, *f, false, false);
        res->addPreAnimation( shared_ptr<DropAnimation>(new DropAnimation(sprite)) );
      }
      else if (c && !f) {
        //current->set(i, NULL);
        SpritePtr old_sprite = m_cinterface->getSprite(i);
        res->addPreAnimation( shared_ptr<CaptureAnimation>(new CaptureAnimation(old_sprite)) );
      }
      else if(c && !c->equals(f) ) {
        //current->set(i, f);
        SpritePtr old_sprite = m_cinterface->takeSprite(i);
        SpritePtr sprite = m_cinterface->setSprite(i, *f, false, false);
        res->addPreAnimation( shared_ptr<PromotionAnimation>(new PromotionAnimation(old_sprite, sprite)) );
      }
    }

    //TODO: implement pool update

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
