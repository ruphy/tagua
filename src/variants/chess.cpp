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

using namespace boost;

const char *ChessVariant::m_name = "Chess";
const char *ChessVariant::m_theme_proxy = "Chess";
VariantInfo* ChessVariant::static_chess_variant = 0;

class ChessAnimator : public SimpleAnimator<ChessVariant> {
public:
  ChessAnimator(PointConverter* converter,
                               const shared_ptr<GPosition>& position)
  : SimpleAnimator<ChessVariant>(converter, position) { }

  shared_ptr<MovementAnimation> 
    createMovementAnimation(const GElement& element, const QPoint& destination) {
      std::cout << "creating movement animation" << std::endl;
      if (element.piece().type() == KNIGHT) {
        std::cout << "rotate = " << m_anim_rotate << std::endl;
        return shared_ptr<MovementAnimation>(
          new KnightMovementAnimation(element.sprite(),
                                      destination, m_anim_rotate));
      }
      else {
        return shared_ptr<MovementAnimation>(
          new MovementAnimation(element.sprite(),
                                destination, 1.0));
      }
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
