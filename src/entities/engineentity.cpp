/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "engineentity.h"
#include "game.h"
#include "algebraicnotation.h"
#include "engine.h"
#include <iostream>

using namespace boost;

EngineEntity::EngineEntity(VariantInfo* variant, const shared_ptr<Game>& game, int side,
                         const shared_ptr<Engine>& engine, AgentGroup* group)
: Entity(game)
, m_variant(variant)
, m_side(side)
, m_engine(engine)
, m_dispatcher(group, this) { }

void EngineEntity::executeMove(AbstractMove::Ptr move) {
  Q_ASSERT(move);
  AbstractPosition::Ptr ref = position();
  AbstractPosition::Ptr pos = ref->clone();
  pos->move(move);
  m_game->add(move, pos);
  m_dispatcher.move(m_game->index());
}

void EngineEntity::notifyEngineMove(const QString& move_str) {
  AbstractMove::Ptr move = position()->getMove(AlgebraicNotation(move_str, position()->size().y));
  if (position()->testMove(move))
    executeMove(move);
  else
    ERROR("Engine attempted to execute an invalid move: " << move_str);
}

void EngineEntity::notifyMove(const Index& index) {
  if (m_side == -1 || m_game->position(index)->turn() == m_side)
    m_engine->sendMove(m_game->move(index), m_game->position(index.prev()));
}




