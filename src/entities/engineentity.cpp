/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "engineentity.h"
#include <KDebug>
#include "game.h"
#include "engine.h"

#include <stack>

using namespace boost;

EngineEntity::EngineEntity(const VariantPtr& variant, const shared_ptr<Game>& game, int side,
                         const shared_ptr<Engine>& engine, AgentGroup* group)
: Entity(game)
, m_variant(variant)
, m_side(side)
, m_last_index(0)
, m_playing(false)
, m_engine(engine)
, m_dispatcher(group, this) { }

void EngineEntity::setup() {
  // setup engine
  m_engine->setNotifier(shared_from_this());
  m_engine->start();
  m_engine->reset();
  
  // find all indexes of the current variation
  std::stack<Index> m_indexes;
  Index index = m_game->index();
  while (index != 0) {
    m_indexes.push(index);
    std::cout << "pushed index " << index << std::endl;
    index = index.prev();
  }
  
  // set engine state playing all moves
  // in the current variation
  while (!m_indexes.empty()) {
    PositionPtr pos = m_game->position(index);
    std::cout <<  "REF:" << std::endl;
    pos->dump();
    
    index = m_indexes.top();
    m_indexes.pop();
    
    MovePtr move = m_game->move(index);
    std::cout << "move = " << move->toString("simple", pos) << std::endl;
    
    m_engine->sendMove(move, pos);
  }
  
  m_last_index = m_game->index();
  checkPlaying();
}

void EngineEntity::notifyEngineMove(const QString& move_str) {
  AbstractMove::Ptr move = position()->getMove(move_str);
  if (position()->testMove(move)) {
    Q_ASSERT(move);
    AbstractPosition::Ptr ref = position();
    AbstractPosition::Ptr pos = ref->clone();
    pos->move(move);
    m_game->add(move, pos);
    m_last_index = m_game->index();
    m_dispatcher.move(m_game->index());
  }
  else
    kError() << "Engine attempted to execute an invalid move:" << move_str;
}

void EngineEntity::checkPlaying() {
  if (!m_playing && m_side == 
      m_game->position(m_last_index)->turn()) {
    m_engine->play();
    m_playing = true;
  }
}

void EngineEntity::notifyMove(const Index& index) {
  if (index == m_last_index) {
    // TODO: check for consistency and update if necessary
  }
  else if (index.prev() == m_last_index) {
    // play move
    m_engine->sendMove(m_game->move(index), m_game->position(index.prev()));
    m_last_index = index;
    checkPlaying();
  }
  else {
    // TODO: handle move notification in arbitrary indexes
    kWarning() << "engine entity can't handle index" << index << "(m_last_index =" << m_last_index << ")";
  }
}




