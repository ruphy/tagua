/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "graphicalsystem.h"
#include "chesstable.h"
#include "board.h"
#include "piecepool.h"
#include "pointconverter.h"
#include "sprite.h"
#include "animation.h"
#include "pref_theme.h"
#include "movelist_table.h"
#include "global.h"

using namespace boost;

//BEGIN GraphicalSystem

GraphicalSystem::GraphicalSystem(ChessTable* view,
                             AbstractPosition::Ptr startingPosition,
                             VariantInfo* variant)
: m_view(view)
, m_variant(variant) {

  m_pos = startingPosition->clone();
  Point s = m_pos->size();
  for(int i=0;i<s.x;i++)
  for(int j=0;j<s.y;j++)
    m_pos->set(Point(i,j), AbstractPiece::Ptr());

  m_board = view->board();

  m_board->createGrid(m_pos->size(), m_pos->borderCoords());
  m_board->reset();

  m_view->pool(0)->clear();
  m_view->pool(1)->clear();

  m_animator = m_variant->createAnimator(this);

  settings.onChange( this, SLOT(settingsChanged()));
  settingsChanged();

  if (startingPosition)
    warp(AbstractMove::Ptr(), startingPosition);
}

GraphicalSystem::~GraphicalSystem() {
}

const PointConverter* GraphicalSystem::converter() const {
  return m_board->converter();
}

AbstractPosition::Ptr GraphicalSystem::position() const {
  return m_pos;
}

void GraphicalSystem::settingsChanged() {

  /* recreate the animator to reload its settings */
  m_animator = m_variant->createAnimator(this);

  QString theme = PrefTheme::getBestTheme(m_variant);
  QString sqtheme = PrefTheme::getBestTheme(m_variant, PrefTheme::Squares);
  QString figtheme = PrefTheme::getBestTheme(m_variant, PrefTheme::Figurines);

  m_board->loader()->setBasePath( theme );
  m_board->tagsLoader()->setBasePath( sqtheme );

  m_view->pool(0)->loader()->setBasePath( theme );
  m_view->pool(1)->loader()->setBasePath( theme );

  m_view->moveListTable()->setLoaderBasePath( figtheme );
  m_view->moveListTable()->settingsChanged();

  //clear board and pool, forcing reload
  m_view->settingsChanged();
}

void GraphicalSystem::setup(const shared_ptr<UserEntity>& entity) {
  m_view->setEntity(entity);
}

NamedSprite GraphicalSystem::getSprite(const Point& p) {
  if (!m_board->m_sprites.valid(p))
    return NamedSprite();

  return m_board->m_sprites[p];
}

NamedSprite GraphicalSystem::takeSprite(const Point& p) {
  if (!m_board->m_sprites.valid(p))
    return NamedSprite();

  NamedSprite retv = m_board->m_sprites[p];
  m_board->m_sprites[p] = NamedSprite();
  return retv;
}

NamedSprite GraphicalSystem::setPiece(const Point& p, const AbstractPiece* piece, bool show) {
  return m_board->m_sprites[p] = createPiece(p, piece, show);
}

NamedSprite GraphicalSystem::createPiece(const Point& p, const AbstractPiece* piece, bool show) {
  Q_ASSERT(piece);
  if (!m_board->m_sprites.valid(p))
    return NamedSprite();

  NamedSprite s(piece->name(), m_board->createSprite(m_board->m_loader(piece->name()), p));
  if (show)
    s.sprite()->show();
  return s;
}

void GraphicalSystem::setSprite(const Point& p, const NamedSprite& sprite) {
  if (!m_board->m_sprites.valid(p))
    return;

  m_board->m_sprites[p] = sprite;
}

int GraphicalSystem::poolSize(int pool) {
  return m_view->pool(pool)->fill();
}

// NamedSprite GraphicalSystem::getPoolSprite(int pool, int index) {
//   return m_view->pool(pool)->getSprite(index);
// }

void GraphicalSystem::removePoolSprite(int pool, int index) {
  m_view->pool(pool)->removeSprite(index);
}

NamedSprite GraphicalSystem::takePoolSprite(int pool, int index) {
  return m_view->pool(pool)->takeSprite(index);
}

NamedSprite GraphicalSystem::insertPoolPiece(int pool, int index, const AbstractPiece* piece) {
  PiecePool *pl = m_view->pool(pool);
  QPixmap px = pl->m_loader(piece->name());

  NamedSprite s( piece->name(), SpritePtr( new Sprite( px, pl->piecesGroup(), QPoint() ) ) );
  pl->insertSprite(index, s);
  return s;
}

std::pair<int, int> GraphicalSystem::droppedPoolPiece() {
  return std::pair<int, int>(m_board->m_dropped_pool, m_board->m_dropped_index);
}

AnimationPtr GraphicalSystem::animate(const Animate::Scheme& scheme, Animate::AnimationType type) {
  return scheme.run(converter(), type);
}


#if 0
void GraphicalSystem::updatePool(AbstractPosition::PoolPtr pool) {

  AbstractPosition::PoolPtr curr = m_pos->pool();

  AbstractPosition::AbstractPool::iterator oldit = curr->begin();
  AbstractPosition::AbstractPool::iterator newit = pool->begin();

  while(oldit != curr->end() || newit != pool->end()) {
    if(newit == pool->end() || (oldit != curr->end()
            && oldit->first->less(newit->first) )) {
      removeFromPool(oldit->first, oldit->second);
      ++oldit;
    }
    else if (oldit == curr->end() || (newit != pool->end()
            && newit->first->less(oldit->first) )) {
      addToPool(newit->first, newit->second);
      ++newit;
    }
    else {
      Q_ASSERT(newit->first->equals(oldit->first));
      if(oldit->second < newit->second)
        addToPool(newit->first, newit->second - oldit->second);
      else if(oldit->second > newit->second)
        removeFromPool(newit->first, oldit->second - newit->second);
      ++newit;
      ++oldit;
    }
  }
}

void GraphicalSystem::addToPool(AbstractPiece::Ptr piece, int n) {
  PiecePool *pool = m_view->pool(!piece->color());
  QPixmap px = pool->m_loader(piece->name());

  for(int i=0;i<n;i++) {
    SpritePtr s = SpritePtr( new Sprite( px, pool->piecesGroup(), QPoint() ) );
    pool->addPiece(Element(piece, s));
  }

  m_pos->addToPool(piece, n);
}

void GraphicalSystem::removeFromPool(AbstractPiece::Ptr piece, int n) {
  PiecePool *pool = m_view->pool(!piece->color());

  for(int i=0;i<n;i++)
    pool->takePiece(piece);
  m_pos->removeFromPool(piece, n);
}
#endif

#if 0
void GraphicalSystem::addTag(const QString& name, Point pt, bool over) {
  m_board->addTag(name, pt, over);
}

void GraphicalSystem::clearTags(const QString& name) {
  m_board->clearTags(name);
}

void GraphicalSystem::setTags(const QString& name, Point p1, Point p2,
                        Point p3, Point p4, Point p5, Point p6 ) {
  m_board->setTags(name, p1, p2, p3, p4, p5, p6);
}
#endif

#if 0
bool GraphicalSystem::consistent() const {
  for (Point i = first(); i <= last(); i = next(i)) {
    Element e = getElement(i);
    if (static_cast<bool>(e.piece()) ^
        static_cast<bool>(e.sprite())) return false;
  }
  return true;
}
#endif

#if 0
Point GraphicalSystem::first() const { return m_board->m_sprites.first(); }
Point GraphicalSystem::last() const { return m_board->m_sprites.last(); }
Point GraphicalSystem::next(const Point& p) const { return m_board->m_sprites.next(p); }
bool GraphicalSystem::valid(const Point& p) const { return m_board->m_sprites.valid(p); }
#endif

void GraphicalSystem::forward(const AbstractMove::Ptr& move,
                              const AbstractPosition::Ptr& pos,
  const SpritePtr& /*movingSprite*/) {
  AbstractPiece::Ptr sel1 = m_pos->get(m_board->selection);

  if (move) {
    AnimationPtr animation = m_animator->forward(pos, move);
    //??? animation->setChainAbortions(false);
    m_board->enqueue(animation);
    m_board->setTags("highlighting", move->toUserMove().from, move->toUserMove().to);

    m_pos->copyFrom(pos);
  }
  else
    warp(AbstractMove::Ptr(), pos);

  AbstractPiece::Ptr sel2 = m_pos->get(m_board->selection);
  if(!(sel1 && sel2 && sel1->equals(sel2)))
    m_board->cancelSelection();

  m_board->cancelPremove();
  m_view->updateTurn(pos->turn());
  m_board->onPositionChanged();
}

void GraphicalSystem::back(const AbstractMove::Ptr& lastMove,
                           const AbstractMove::Ptr& move,
                           const AbstractPosition::Ptr& pos) {
  AbstractPiece::Ptr sel1 = m_pos->get(m_board->selection);

  if (move) {
    AnimationPtr animation = m_animator->back(pos, move);
    //??? animation->setChainAbortions(false);
    m_board->enqueue(animation);

    m_pos->copyFrom(pos);
  }
  else
    warp(lastMove, pos);

  if (lastMove)
    m_board->setTags("highlighting", lastMove->toUserMove().from, lastMove->toUserMove().to);
  else
    m_board->clearTags("highlighting");

  AbstractPiece::Ptr sel2 = m_pos->get(m_board->selection);
  if(!(sel1 && sel2 && sel1->equals(sel2)))
    m_board->cancelSelection();
  m_board->cancelPremove();
  m_view->updateTurn(pos->turn());
  m_board->onPositionChanged();
}

void GraphicalSystem::warp(const AbstractMove::Ptr& lastMove,
                           const AbstractPosition::Ptr& pos) {

  AbstractPiece::Ptr sel1 = m_pos->get(m_board->selection);

  AnimationPtr animation = m_animator->warp(pos);
  //??? animation->setChainAbortions(false);
  m_board->enqueue(animation);

  m_pos->copyFrom(pos);

  if (lastMove)
    m_board->setTags("highlighting", lastMove->toUserMove().from, lastMove->toUserMove().to);
  else
    m_board->clearTags("highlighting");

  AbstractPiece::Ptr sel2 = m_pos->get(m_board->selection);
  if(!(sel1 && sel2 && sel1->equals(sel2)))
    m_board->cancelSelection();
  m_view->updateTurn(pos->turn());
  m_board->onPositionChanged();
}

void GraphicalSystem::adjustSprite(const Point& p) {
  SpritePtr sprite = m_board->m_sprites[p].sprite();
  Q_ASSERT(sprite);
  QPoint destination = m_board->converter()->toReal(p);
  shared_ptr<Animation> animation(new MovementAnimation(sprite, destination));
  m_board->enqueue(animation);
}

void GraphicalSystem::setTurn(int turn) {
  m_pos->setTurn(turn);
  m_view->updateTurn(m_pos->turn());
}





