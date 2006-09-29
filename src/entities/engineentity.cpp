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

EngineEntity::EngineEntity(VariantInfo* variant, const shared_ptr<Game>& game,
                         const shared_ptr<Engine>& engine, AgentGroup* group)
: GameBasedEntity(game)
, m_variant(variant)
, m_engine(engine)
, m_dispatcher(group, this) { }

void EngineEntity::executeMove(AbstractMove::Ptr move) {
  Q_ASSERT(move);
  AbstractPosition::Ptr ref = position();
  AbstractPosition::Ptr pos = ref->clone();
  pos->move(move);
  m_game->add(move, pos);
  m_dispatcher.move(move, ref);
}

void EngineEntity::notifyEngineMove(const QString& move_str) {
  AbstractMove::Ptr move = position()->getMove(AlgebraicNotation(move_str, position()->size().y));
  if (position()->testMove(move))
    executeMove(move);
  else
    std::cout << "** engine attempted to execute an invalid move **" << std::endl;
}

void EngineEntity::notifyMove(AbstractMove::Ptr mv, AbstractPosition::Ptr ref) {
  m_engine->sendMove(mv, ref);
}




