/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "crazyhouse_p.h"
#include "crazyhouse.h"
#include "piecesprite.h"

//BEGIN CrazyhousePiece

CrazyhousePiece::CrazyhousePiece(ChessPiece::Color color, ChessPiece::Type type, bool promoted)
: ChessPiece(color, type)
, m_promoted(promoted) { }

CrazyhousePiece::CrazyhousePiece(const CrazyhousePiece& other)
: ChessPiece(other)
, m_promoted(other.m_promoted) { }

CrazyhousePiece::CrazyhousePiece(const ChessPiece& other)
: ChessPiece(other)
, m_promoted(false) { }

//END CrazyhousePiece

//BEGIN CrazyhouseMove

CrazyhouseMove::CrazyhouseMove(const ChessMove& m)
: ChessMove(m)
, m_drop(INVALID_COLOR, INVALID_TYPE) { }

CrazyhouseMove::CrazyhouseMove(const Point& from, const Point& to, PieceType promotionType)
: ChessMove(from, to, promotionType)
, m_drop(INVALID_COLOR, INVALID_TYPE) { }

CrazyhouseMove::CrazyhouseMove(const CrazyhouseMove& other)
: ChessMove(other)
, m_drop(other.m_drop) { }

CrazyhouseMove::CrazyhouseMove(CrazyhousePiece piece, const Point& to)
: ChessMove(Point::invalid(), to)
, m_drop(piece) { }

//END CrazyhouseMove

//BEGIN CrazyhousePosition

CrazyhousePosition::CrazyhousePosition() {
}

CrazyhousePosition::CrazyhousePosition(const OptList&) {
}

CrazyhousePosition::CrazyhousePosition(const CrazyhousePosition& other)
: Base(other) {
}

CrazyhousePosition::CrazyhousePosition(const ChessPosition& other)
: Base(other) {
}

CrazyhousePosition::CrazyhousePosition(CrazyhousePiece::Color turn, bool wk, bool wq,
                                          bool bk, bool bq, const Point& ep)
: Base(turn, wk, wq, bk, bq, ep) {
}

CrazyhousePosition* CrazyhousePosition::clone() const {
  return new CrazyhousePosition(*this);
}

CrazyhousePiece::Color CrazyhousePosition::moveTurn(const Move& move) const {
  if (move.m_drop.valid() )
    return move.m_drop.color();
  else
    return Base::moveTurn(move);
}

boost::shared_ptr<AbstractGenerator<CrazyhouseMove> >
CrazyhousePosition::createLegalGenerator() const {
  return boost::shared_ptr<AbstractGenerator<CrazyhouseMove> >(
    new MoveGenerator<CrazyhousePosition,
      LegalMove<CrazyhousePosition> >(*this));
}

bool CrazyhousePosition::pseudolegal(Move& move) const {

  if (move.m_drop.valid()) {
    Q_ASSERT(valid(move.to));

    // cannot drop on occupied squares
    if (m_board[move.to]) return false;

    // cannot drop pawns in first or eighth rank
    if (move.m_drop.type() == PAWN &&
        (move.to.y == 0 || move.to.y == 7))
      return false;

    return true;
  }
  // normal move
  else
    return Base::pseudolegal(move);
}

void CrazyhousePosition::move(const Move& move) {
  // drop
  if (move.m_drop.valid()) {
    Q_ASSERT(m_pool.count(move.m_drop));
    Q_ASSERT(!m_board[move.to]);

    basicDropPiece(new Piece(move.m_drop), move.to);
    if(!--m_pool[move.m_drop])
      m_pool.erase(move.m_drop);
  }
  else {
    // normal move
    Base::move(move);

    // set promoted flag
    if (move.type() == Move::Promotion) {
      m_board[move.to].setPromoted(true);
    }
  }

#if 0
  for(Pool::iterator i = m_pool.begin(); i != m_pool.end(); ++i)
    std::cout << i->first.color() << "." << i->first.type() << " " << i->second <<std::endl;
#endif
}

void CrazyhousePosition::executeCaptureOn(const Point& point) {
  Piece piece = m_board[point];
  if (piece) {
    Piece downgraded( Piece::oppositeColor(piece.color()),
                piece.promoted() ? PAWN : piece.type());
    m_pool[downgraded]++;
  }
  Base::executeCaptureOn(point);
}

CrazyhouseMove CrazyhousePosition::getMove(const AlgebraicNotation& san, bool& ok) const {

  if (san.drop) {
    Piece piece(m_turn, (PieceType)san.type);

    if ( !m_pool.count( piece )) {
      ok = false;
      return CrazyhouseMove::invalid();
    }
    else {
      ok = true;
      return CrazyhouseMove( piece, san.to);
    }
  }
  else
    return Base::getMove(san, ok);
}

void CrazyhousePosition::dump() const {
  Base::dump();

  for(Pool::const_iterator i = m_pool.begin(); i != m_pool.end(); ++i)
    std::cout << i->first.color() << "." << i->first.type() << " " << i->second <<std::endl;
}

bool CrazyhousePosition::operator==(const CrazyhousePosition& pos) const {
  return pool() == pos.pool() && Base::operator==(pos);
}

//END CrazyhousePosition

#include "animation.h"
#include "graphicalposition.h"
#include "pointconverter.h"

//BEGIN CrazyhouseAnimator


CrazyhouseAnimator::CrazyhouseAnimator(PointConverter* converter, GraphicalPosition* position)
: ChessAnimator(converter, position) {
}

CrazyhouseAnimator::AnimationPtr CrazyhouseAnimator::warp(AbstractPosition::Ptr final) {
  //std::cout << "Crazy warp!!!" << std::endl;
  m_position->updatePool(final->pool());

  return ChessAnimator::warp(final);
}

CrazyhouseAnimator::AnimationPtr CrazyhouseAnimator::forward(AbstractPosition::Ptr final,
                                                      const CrazyhouseMove& move) {
  if(move.m_drop.valid()) {
    AnimationPtr res(new AnimationGroup);
    AbstractPiece::Ptr drop = final->get(move.to);
    boost::shared_ptr<PieceSprite> drop_sprite = m_position->setPiece(move.to, drop, true);

    if(!drop_sprite->visible())
      res->addPreAnimation(
        m_anim_fade
        ? boost::shared_ptr<Animation>(new FadeAnimation(drop_sprite,
                                      m_converter->toReal(move.to), 0, 255 ))
        : boost::shared_ptr<Animation>(new DropAnimation(drop_sprite))
      );
    else
      res->addPreAnimation(
        m_anim_movement
        ? boost::shared_ptr<Animation>(new MovementAnimation(drop_sprite,
                                            m_converter->toReal(move.to)))
        : boost::shared_ptr<Animation>(new InstantAnimation(drop_sprite,
                                            m_converter->toReal(move.to)))
      );

    res->addPostAnimation(warp(final));
    return res;
  }
  else
    return ChessAnimator::forward(final, move);
}

CrazyhouseAnimator::AnimationPtr CrazyhouseAnimator::back(AbstractPosition::Ptr final,
                                                      const CrazyhouseMove& move) {
  if(move.m_drop.valid()) {
    AnimationPtr res(new AnimationGroup);
    boost::shared_ptr<PieceSprite> drop_sprite = m_position->getSprite(move.to);
    m_position->removeElement(move.to);
    boost::shared_ptr<FadeAnimation> fadeoff(new FadeAnimation(drop_sprite,
                                                m_converter->toReal(move.to), 255, 0));
    res->addPreAnimation(
      m_anim_fade
      ? boost::shared_ptr<Animation>(new FadeAnimation(drop_sprite,
                                       m_converter->toReal(move.to), 255, 0))
      : boost::shared_ptr<Animation>(new CaptureAnimation(drop_sprite))
    );
    res->addPostAnimation(warp(final));
    return res;
  }
  else
    return ChessAnimator::back(final, move);
}

//END CrazyhouseAnimator

//BEGIN CrazyhouseVariant


#include "highlevel.h"
#include "moveserializer.impl.h"

class CrazyhouseVariantInfo {
public:
  typedef CrazyhousePosition Position;
  typedef Position::Move Move;
  typedef Position::Piece Piece;
  typedef CrazyhouseAnimator Animator;
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

const char *CrazyhouseVariantInfo::m_name = "Crazyhouse";
const char *CrazyhouseVariantInfo::m_theme_proxy = "Chess";

VariantInfo* CrazyhouseVariant::static_crazyhouse_variant = 0;

void CrazyhouseVariantInfo::forallPieces(PieceFunction& f) {
  return ChessVariant::forallPieces(f);
}

VariantInfo* CrazyhouseVariant::info() {
  if (!static_crazyhouse_variant)
    static_crazyhouse_variant = new WrappedVariantInfo<CrazyhouseVariantInfo>;
  return static_crazyhouse_variant;
}

//END CrazyhouseVariant
