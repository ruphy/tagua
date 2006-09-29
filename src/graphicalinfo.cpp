/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "graphicalinfo.h"
#include "chesstable.h"
#include "board.h"
#include "piecepool.h"
#include "pointconverter.h"
#include "piecesprite.h"
#include "animation.h"
#include "pref_theme.h"
#include "settings.h"

using namespace boost;

void GraphicalInfoProxy::settingsChanged() {
  m_info->settingsChanged();
}

//BEGIN GraphicalInfo

GraphicalInfo::GraphicalInfo(ChessTable* view,
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

  m_board->createGrid(m_pos->size(), m_variant->borderCoords());
  m_board->reset();

  m_view->pool(0)->clear();
  m_view->pool(1)->clear();

  m_animator = m_variant->createAnimator(m_board->converter(), this);

  m_proxy = new GraphicalInfoProxy(this);
  settings.onChange(m_proxy, SLOT(settingsChanged()));
  settingsChanged();

  if (startingPosition)
    warp(AbstractMove::Ptr(), startingPosition);
}

GraphicalInfo::~GraphicalInfo() {
  delete m_proxy;
}

void GraphicalInfo::settingsChanged() {

  /* recreate the animator to reload its settings */
  m_animator = m_variant->createAnimator(m_board->converter(), this);

  QString theme = PrefTheme::getBestTheme(m_variant);
  QString sqtheme = PrefTheme::getBestTheme(m_variant, true);

  m_board->loader()->setBasePath( theme );
  m_board->tagsLoader()->setBasePath( sqtheme );

  m_view->pool(0)->loader()->setBasePath( theme );
  m_view->pool(1)->loader()->setBasePath( theme );

  //clear board and pool, forcing reload
  m_view->settingsChanged();
}

void GraphicalInfo::setup(const shared_ptr<UserEntity>& entity) {
  m_view->setEntity(entity);
}

int GraphicalInfo::getIntSetting(const QString& key, int def_value) const {
  return (settings[key] | def_value).value<int>();
}

bool GraphicalInfo::getBoolSetting(const QString& key, bool def_value) const {
  return (settings[key] | def_value).value<bool>();
}

QString GraphicalInfo::getStringSetting(const QString& key, const QString& def_value) const {
  return (settings[key] | def_value).value<QString>();
}

GraphicalInfo::SpritePtr GraphicalInfo::getSprite(const Point& p) const {
  if(!m_board->m_sprites.valid(p))
    return shared_ptr<PieceSprite>();

  return m_board->m_sprites[p].sprite();
}

GraphicalInfo::SpritePtr GraphicalInfo::setPiece(const Point& p,
                              AbstractPiece::Ptr piece, bool usedrop, bool show) {
  Q_ASSERT(piece);
  if(!m_board->m_sprites.valid(p)) {
    printf("que?!? %d %d\n", p.x, p.y);
    return SpritePtr();
  }

  m_pos->set(p, piece);
  QPixmap px = m_board->m_loader(piece->name());

  SpritePtr s;
  if(usedrop && m_board->m_drop_sprite &&
        m_board->m_drop_sprite.piece()->equals(piece) ) {
    s = m_board->m_drop_sprite.sprite();
    m_board->m_drop_sprite = Element();
  }
  else {
    s = m_board->createSprite(px, p);
    if (show) s->show();
  }
  m_board->m_sprites[p] = Board::BoardSprite(piece->name(), s);
  return s;
}

Element GraphicalInfo::getElement(const Point& p) const {
  if(!m_board->m_sprites.valid(p))
    return Element();

  return Element(m_pos->get(p), m_board->m_sprites[p].sprite());
}

void GraphicalInfo::setElement(const Point& p, const Element& e) {
  if(!m_board->m_sprites.valid(p))
    return;

  AbstractPiece::Ptr piece = e.piece();
  m_pos->set(p, piece);
  m_board->m_sprites[p] = Board::BoardSprite( piece ? piece->name() : 0, e.sprite() );
}

void GraphicalInfo::removeElement(const Point& p) {
  if(!m_board->m_sprites.valid(p))
    return;

  setElement(p, Element());
}

void GraphicalInfo::updatePool(AbstractPosition::PoolPtr pool) {

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

void GraphicalInfo::addToPool(AbstractPiece::Ptr piece, int n) {
  PiecePool *pool = m_view->pool(!piece->color());
  QPixmap px = pool->m_loader(piece->name());

  for(int i=0;i<n;i++) {
    SpritePtr s = SpritePtr( new PieceSprite( px, pool->piecesGroup(), QPoint() ) );
    pool->addPiece(Element(piece, s));
  }
  m_pos->addToPool(piece, n);
}

void GraphicalInfo::removeFromPool(AbstractPiece::Ptr piece, int n) {
  PiecePool *pool = m_view->pool(!piece->color());

  for(int i=0;i<n;i++)
    pool->takePiece(piece);
  m_pos->removeFromPool(piece, n);
}

void GraphicalInfo::addTag(const QString& name, Point pt, bool over) {
  m_board->addTag(name, pt, over);
}

void GraphicalInfo::clearTags(const QString& name) {
  m_board->clearTags(name);
}

void GraphicalInfo::setTags(const QString& name, Point p1, Point p2,
                        Point p3, Point p4, Point p5, Point p6 ) {
  m_board->setTags(name, p1, p2, p3, p4, p5, p6);
}

bool GraphicalInfo::consistent() const {
  for (Point i = first(); i <= last(); i = next(i)) {
    Element e = getElement(i);
    if (static_cast<bool>(e.piece()) ^
        static_cast<bool>(e.sprite())) return false;
  }
  return true;
}

Point GraphicalInfo::first() const { return m_board->m_sprites.first(); }
Point GraphicalInfo::last() const { return m_board->m_sprites.last(); }
Point GraphicalInfo::next(const Point& p) const { return m_board->m_sprites.next(p); }
bool GraphicalInfo::valid(const Point& p) const { return m_board->m_sprites.valid(p); }


void GraphicalInfo::forward(AbstractMove::Ptr move, AbstractPosition::Ptr pos,
  const shared_ptr<PieceSprite>& /*movingSprite*/) {
  AbstractPiece::Ptr sel1 = m_pos->get(m_board->selection);

  if (move) {
    shared_ptr<AnimationGroup> animation = m_animator->forward(pos, move);
    animation->setChainAbortions(false);
    m_board->enqueue(animation);
    m_board->setTags("highlighting", move->toUserMove().from, move->toUserMove().to);
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

void GraphicalInfo::back(AbstractMove::Ptr lastMove,
                         AbstractMove::Ptr move,
                         AbstractPosition::Ptr pos) {
  AbstractPiece::Ptr sel1 = m_pos->get(m_board->selection);

  if (move) {
    shared_ptr<AnimationGroup> animation = m_animator->back(pos, move);
    animation->setChainAbortions(false);
    m_board->enqueue(animation);
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

void GraphicalInfo::warp(AbstractMove::Ptr lastMove, AbstractPosition::Ptr pos) {

  AbstractPiece::Ptr sel1 = m_pos->get(m_board->selection);
  shared_ptr<AnimationGroup> animation = m_animator->warp(pos);
  animation->setChainAbortions(false);

  m_board->enqueue(animation);

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

void GraphicalInfo::adjustSprite(const Point& p) {
  SpritePtr sprite = m_board->m_sprites[p].sprite();
  Q_ASSERT(sprite);
  QPoint destination = m_board->converter()->toReal(p);
  shared_ptr<Animation> animation(new MovementAnimation(sprite, destination));
  m_board->enqueue(animation);
}

void GraphicalInfo::setTurn(int turn) {
  m_pos->setTurn(turn);
  m_view->updateTurn(m_pos->turn());
}

#include "graphicalinfo.moc"
