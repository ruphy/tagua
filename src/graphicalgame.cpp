/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include "graphicalgame.h"
#include "game.h"
#include "game_p.h"
#include "global.h"
#include "graphicalsystem.h"
#include "movelist_table.h"
#include "decoratedmove.h"
#include "entities/userentity.h"
#include <iostream>

using namespace GamePrivate; // is this ok?

class CtrlAction {
  Game* m_game;
  bool m_done;
  Index m_index;
public:
  CtrlAction(Game* game)
  : m_game(game)
  , m_index(game->index()) {
    m_done = m_game->back();
  }

  void forfait() { m_done = false; }

  ~CtrlAction() {
    if (m_done) m_game->goTo(m_index);
  }
};

GraphicalGame::GraphicalGame(GraphicalSystem* graphical,
                             MoveList::Table* m)
: Game()
, m_graphical(graphical)
, m_movelist(m)
, m_anim_sequence(false) {
  if(m_movelist) {
    m_movelist->reset();
    m_movelist->setLayoutStyle(graphical->m_variant->moveListLayout());
    m_movelist->setNotifier( static_cast<MoveList::Notifier*>(this) );
    m_movelist->show();
  }
  settings.onChange(this, SLOT(settingsChanged()));
  settingsChanged();
}

GraphicalGame::~GraphicalGame() {
  if(m_movelist) {
    Q_ASSERT(m_movelist->getNotifier() == static_cast<MoveList::Notifier*>(this));

    m_movelist->setNotifier(NULL, false);
  }
}

void GraphicalGame::settingsChanged() {
  //BROKEN
  #if 0
  m_anim_sequence = m_graphical->getBoolSetting("animations", true)
                     && m_graphical->getBoolSetting("animations.sequence", true);
  m_anim_sequence_max = m_graphical->getIntSetting("animations.sequence.max", 10);
  #endif
}

void GraphicalGame::onAdded(const Index& ix) {
  onAddedInternal(ix);
}

void GraphicalGame::onAddedInternal(const Index& ix, bool confirm_promotion) {
  if(!m_movelist)
    return;

  int at;
  History *vec = fetchRef(ix, &at);
  if(!vec) {
    std::cout << "--> Error in GraphicalGame::onAdded, invalid index "<< ix << std::endl;
    return;
  }

  m_movelist->remove(ix, confirm_promotion); //clear existing, if any

  Index index = ix;
  for(int i=at;i<(int)vec->size();i++) {
    Entry* e = &(*vec)[i];
    PositionPtr prev = position(index.prev());
    DecoratedMove mv = (e->move && prev) ? e->move->toDecoratedMove(prev) :
                  DecoratedMove() << MovePart(e->position ? "(-)" : "???");
    int turn = prev ? prev->turn() : (index.totalNumMoves()+1)%2;
    //mv += " " + QString::number(turn);
    m_movelist->setMove(index, turn, mv, e->comment, confirm_promotion);

    for (Variations::const_iterator it = e->variations.begin();
            it != e->variations.end(); ++it)
      onAddedInternal(index.next(it->first), confirm_promotion);
    for (VComments::const_iterator it = e->vcomments.begin();
            it != e->vcomments.end(); ++it)
      m_movelist->setVComment(index, it->first, it->second, confirm_promotion);

    index = index.next();
  }
}

void GraphicalGame::onEntryChanged(const Index& at, int propagate) {
  if(at <= Index(0)) {
    Entry* e = fetch(at);
    if(!e)
      return;
    if(at == current && e->position)
      m_graphical->warp(e->move, e->position);
    return;
  }

  if(m_movelist) {
    Entry* e = fetch(at);
    if(!e)
      return;

    Entry* pe = fetch(at.prev());

    AbstractPosition::Ptr last_pos;
    if (pe) last_pos = pe->position;

    DecoratedMove mv = (e->move && last_pos) ? e->move->toDecoratedMove(last_pos) :
                    DecoratedMove() << MovePart(e->position ? "(-)" : "???");
    int turn = last_pos ? last_pos->turn() : (at.totalNumMoves()+1)%2;
    m_movelist->setMove(at, turn, mv, e->comment);
    if(at == current && e->position)
      m_graphical->warp(e->move, e->position);

    // when an entry changes, chances are that we get some more information about the
    // next ones as well
    if(propagate) {
      onEntryChanged(at.next(), propagate-1);
      for (Variations::const_iterator it = e->variations.begin();
              it != e->variations.end(); ++it)
        onEntryChanged(at.next(it->first), propagate-1);
    }
  }
}

void GraphicalGame::onRemoved(const Index& i) {
  if(m_movelist)
    m_movelist->remove(i);
}

void GraphicalGame::onPromoteVariation(const Index& i, int v) {
  if(m_movelist) {
    m_movelist->promoteVariation(i,v);
    onAddedInternal(i.next(), true);
    onAddedInternal(i.next(v), true);
    VComments vc = fetch(i)->vcomments;
    VComments::const_iterator it = vc.find(v);
    if(it != vc.end())
      m_movelist->setVComment(i, v, it->second, true);
    m_movelist->select(current, true);
  }
}

void GraphicalGame::onSetComment(const Index& i, const QString& s) {
  if(m_movelist)
    m_movelist->setComment(i, s);
}

void GraphicalGame::onSetVComment(const Index& i, int v, const QString& s) {
  if(m_movelist)
    m_movelist->setVComment(i, v, s);
}

void GraphicalGame::onCurrentIndexChanged(const Index& old_c) {
  if (m_ctrl) m_ctrl->forfait();

  if(m_movelist)
    m_movelist->select(current);

  Entry *oe = fetch(old_c);
  Entry *e = fetch(current);
  std::pair<int, int> steps = old_c.stepsTo(current);

  if(!e || !e->position)
    return;

  if(!oe || !oe->position) {
    m_graphical->warp(move(), position());
    return;
  }

  bool can_animate = (steps.first+steps.second <= 1) || (m_anim_sequence
                    && (steps.first+steps.second <= m_anim_sequence_max));

  if(can_animate)
  for(int i=1;i<=steps.first;i++)
  if( !move(old_c.prev(i-1)) || !position(old_c.prev(i))) {
    can_animate = false;
    break;
  }

  if(can_animate)
  for(int i=steps.second-1;i>=0;i--)
  if( !move(current.prev(i)) || !position(current.prev(i))) {
    can_animate = false;
    break;
  }

  if(can_animate) {
    for(int i=1;i<=steps.first;i++)
      m_graphical->back( move(old_c.prev(i)), move(old_c.prev(i-1)), position(old_c.prev(i)));
    for(int i=steps.second-1;i>=0;i--)
      m_graphical->forward( move(current.prev(i)), position(current.prev(i)));
  }
  else
    m_graphical->warp( move(), position());
}

void GraphicalGame::onAvailableUndo(bool e) {
  if(m_movelist)
    m_movelist->enableUndo(e);
}

void GraphicalGame::onAvailableRedo(bool e) {
  if(m_movelist)
    m_movelist->enableRedo(e);
}

void GraphicalGame::onUserSelectMove(const Index& i) {
  if (boost::shared_ptr<UserEntity> entity = m_listener_entity.lock())
    if (entity->goTo(i))
      return;

  // fallback
  goTo(i);
}

void GraphicalGame::onUserSetComment(const Index& i, QString s) {
  setComment(i, s);
}

void GraphicalGame::onUserSetVComment(const Index& i, int v, QString s) {
  setVComment(i, v, s);
}

void GraphicalGame::onUserClearVariations(const Index& i) {
  clearVariations(i);
}

void GraphicalGame::onUserTruncate(const Index& i) {
  if (i == index())
    if (boost::shared_ptr<UserEntity> entity = m_listener_entity.lock())
      if (entity->truncate())
        return;

  // fallback
  truncate(i);
}

void GraphicalGame::onUserPromoteVariation(const Index& i) {
  if (i == index())
    if (boost::shared_ptr<UserEntity> entity = m_listener_entity.lock())
      if (entity->promoteVariation())
        return;

  // fallback
  promoteVariation(i);
}

void GraphicalGame::onUserRemoveVariation(const Index& i) {
  removeVariation(i);
}

void GraphicalGame::onUserUndo() {
  if (boost::shared_ptr<UserEntity> entity = m_listener_entity.lock())
    if (entity->undo())
      return;

  // fallback
  undo();
}

void GraphicalGame::onUserRedo() {
  if (boost::shared_ptr<UserEntity> entity = m_listener_entity.lock())
    if (entity->redo())
      return;

  // fallback
  redo();
}

void GraphicalGame::onDetachNotifier() {
  Q_ASSERT(m_movelist->getNotifier() == static_cast<MoveList::Notifier*>(this));

  m_movelist = NULL;
}

void GraphicalGame::createCtrlAction() {
  m_ctrl = boost::shared_ptr<CtrlAction>(new CtrlAction(this));
}

void GraphicalGame::destroyCtrlAction() {
  m_ctrl.reset();
}

#include "graphicalgame.moc"
