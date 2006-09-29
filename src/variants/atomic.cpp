/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "atomic.h"
#include "highlevel.h"
#include "xchess/position.h"
#include "moveserializer.impl.h"
#include "xchess/animator.h"
#include "graphicalposition.h"
#include "pointconverter.h"
#include "animation.h"

#define USE_DELAYED_EXPLOSIONS

using namespace boost;

class AtomicPosition : public ChessPosition {
public:
  typedef ChessPosition::Piece Piece;
  AtomicPosition();
  AtomicPosition(const OptList& l);
  AtomicPosition(const ChessPosition&);
  AtomicPosition(Piece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep);
  virtual AtomicPosition* clone() const { return new AtomicPosition(*this); }
  virtual bool attacks(Color, const Point& destination) const;
  virtual bool attacks(Color, const Point& destination, Point& source) const;
  virtual void move(const ChessMove&);
  virtual bool testMove(ChessMove&) const;
  virtual bool pseudolegal(ChessMove&) const;
  virtual QString type() const { return "atomic"; }
};

AtomicPosition::AtomicPosition() { }

AtomicPosition::AtomicPosition(const OptList&) { }

AtomicPosition::AtomicPosition(const ChessPosition& other)
: ChessPosition(other) { }

AtomicPosition::AtomicPosition(Piece::Color turn, bool wk, bool wq,
                               bool bk, bool bq, const Point& ep)
: ChessPosition(turn, wk, wq, bk, bq, ep) { }

bool AtomicPosition::attacks(Color color, const Point& destination) const {
  Point source;
  if (ChessPosition::attacks(color, destination)) {
    if (!m_board[destination]) return true;

    // a piece does not attack occupied squares near its own king
    Piece king(color, KING);
    for (int i = -1; i <= 1; i++)
    for (int j = -1; j <= 1; j++) {
      Point p = destination + Point(i,j);
      if (valid(p) && king.equals(m_board[p])) return false;
    }

    return true;
  }
  else return false;
}

bool AtomicPosition::attacks(Color color, const Point& destination, Point& source) const {
  bool b = ChessPosition::attacks(color, destination, source);
  printf("VAL: %d!\n",b);
  return b;
}

void AtomicPosition::move(const ChessMove& mv) {
  bool cap = m_board[mv.to];
  ChessPosition::move(mv);

  if (!cap) return;

  for (int i = -1; i <= 1; i++)
  for (int j = -1; j <= 1; j++) {
    Point p = mv.to + Point(i,j);
    if (!valid(p)) continue;
    Piece* piece = m_board[p];
    if (piece && (piece->type() != PAWN ||
                  (i == 0 && j == 0))) {
      delete m_board[p];
      m_board[p] = 0;
    }
  }
}

bool AtomicPosition::pseudolegal(ChessMove& mv) const {
  if (!ChessPosition::pseudolegal(mv)) return false;

  // a move that causes the explosion of one's king
  // is not pseudolegal
  Piece::Color color = m_board[mv.from]->color();

  { std::auto_ptr<AtomicPosition> temp(clone());

    temp->move(mv);
    if (temp->findPiece(color, KING) == Point::invalid())
    {
      printf("CUSP!\n");
      return false;
    }
  }

  return true;
}

bool AtomicPosition::testMove(ChessMove& move) const {
  if (move.status == ChessMove::Untested) {
    if (pseudolegal(move)) {
      ChessPiece::Color turn = moveTurn(move);

      // check king safety
      std::auto_ptr<AtomicPosition> tempPosition(clone());
      std::cout << "[atomic_position] cloned" << std::endl;
      tempPosition->move(move);
      std::cout << "[atomic_position] move tried" << std::endl;
      Point kingPos = tempPosition->findPiece(turn, KING);
      std::cout << "[atomic_position] found king: " << kingPos << std::endl;
      Point kingPos2 = tempPosition->findPiece(ChessPiece::oppositeColor(turn), KING);
      std::cout << "[atomic_position] found king: " << kingPos << std::endl;

      if (kingPos == Point::invalid())
        move.status = ChessMove::Illegal;
      else if (kingPos2 == Point::invalid())
        move.status = ChessMove::Legal;
      else if (tempPosition->attacks(ChessPiece::oppositeColor(turn), kingPos))
          move.status = ChessMove::Illegal;
      else
          move.status = ChessMove::Legal;
    }
    else move.status = ChessMove::Illegal;
  }

  return move.status == ChessMove::Legal ? true : false;
}


class AtomicAnimator : public ChessAnimator {
public:
  AtomicAnimator(PointConverter* converter, GraphicalPosition* position)
  : ChessAnimator(converter, position) { }

  virtual shared_ptr<Animation> createCapture(
            const Point& point,
            const Element& piece,
            const Element& captured,
            AbstractPosition::Ptr pos) {
    if(m_anim_explode) {
#ifdef USE_DELAYED_EXPLOSIONS
      shared_ptr<DelayedAnimationSet> res(new DelayedAnimationSet(m_random));
#else // USE_DELAYED_EXPLOSIONS
#define addAnimation addPreAnimation
      AnimationPtr res(new AnimationGroup);
#endif // USE_DELAYED_EXPLOSIONS

      res->addAnimation(ChessAnimator::createCapture(point, piece, captured, pos));

      if(captured) {
        // add explosions
        for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
          if (i == 0 && j == 0) continue;
          Point p = point + Point(i,j);
          Element element = m_position->getElement(p);
          if (!pos->get(p) && element) {
            m_position->removeElement(p);
            res->addAnimation(shared_ptr<Animation>(
                new ExplodeAnimation(element.sprite(), m_random)));
          }
        }

        // explode the capturing piece too :)
        res->addAnimation(shared_ptr<Animation>(
                    new ExplodeAnimation(piece.sprite(), m_random)));
      }

      return res;
    }
    else {
      AnimationPtr res(new AnimationGroup);

      res->addPreAnimation(ChessAnimator::createCapture(point, piece, captured, pos));

      if(captured) {
        // add explosions
        for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
          if (i == 0 && j == 0) continue;
          Point p = point + Point(i,j);
          Element element = m_position->getElement(p);
          if (!pos->get(p) && element) {
            m_position->removeElement(p);
            res->addPreAnimation(
              m_anim_fade
              ? shared_ptr<Animation>(new FadeAnimation(element.sprite(),
                                        m_converter->toReal(p), 255, 0))
              : shared_ptr<Animation>(new CaptureAnimation(element.sprite()))
            );
          }
        }

        // explode the capturing piece too :)
        res->addPreAnimation(
          m_anim_fade
          ? shared_ptr<Animation>(new FadeAnimation(piece.sprite(),
                                      m_converter->toReal(point), 255, 0))
          : shared_ptr<Animation>(new CaptureAnimation(piece.sprite()))
        );
      }

      return res;
    }
  }

  virtual void finalizeBackAnimation(AnimationPtr animation,
                                     AbstractPosition::Ptr pos,
                                     const ChessMove& move) {
    // undo explosions
    for (int i = -1; i <= 1; i++)
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) continue;
      Point p = move.to + Point(i,j);
      Element element = m_position->getElement(p);
      if (pos->get(p) && !element) {
        QPoint real = m_converter->toReal(p);
        animation->addPreAnimation(
          m_anim_fade
          ? shared_ptr<Animation>(new FadeAnimation(
                  m_position->setPiece(p, pos->get(p)), real, 0, 255))
          : shared_ptr<Animation>(new DropAnimation(
                  m_position->setPiece(p, pos->get(p))))
        );
      }
    }
  }
};


class AtomicVariantInfo {
public:
  typedef AtomicPosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef AtomicAnimator Animator;
  static const bool m_simple_moves = false;
  static void forallPieces(PieceFunction& f);
  static int moveListLayout(){ return 0; }
  static QStringList borderCoords(){
    return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                       << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
  }
  static const char *m_name;
  static const char *m_theme_proxy;
  static OptList positionOptions() { return OptList(); }
};

const char *AtomicVariantInfo::m_name = "Atomic";
const char *AtomicVariantInfo::m_theme_proxy = "Chess";

void AtomicVariantInfo::forallPieces(PieceFunction& f) {
  ChessVariant::forallPieces(f);
}


VariantInfo* AtomicVariant::static_atomic_variant = 0;

VariantInfo* AtomicVariant::info() {
  if (!static_atomic_variant)
    static_atomic_variant = new WrappedVariantInfo<AtomicVariantInfo>;
  return static_atomic_variant;
}

