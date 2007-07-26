/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "editgame.h"
#include "graphicalgame.h"
#include "graphicalsystem.h"
#include "xboardengine.h"
#include "entities/engineentity.h"
#include "entities/gameentity.h"
#include "entities/icsentity.h"
#include "entities/examinationentity.h"
#include "movelist_table.h"
#include "console.h"
#include "icsconnection.h"
#include "chesstable.h"
#include "foreach.h"
#include <iostream>

using namespace boost;

class ClockAgent : public Agent {
  ChessTable* m_view;
  shared_ptr<Game> m_game;
public:
  ClockAgent(ChessTable* view, const shared_ptr<Game>& game)
  : m_view(view)
  , m_game(game) { }

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(AbstractMove::Ptr, AbstractPosition::Ptr) {
    Index index = m_game->lastMainlineIndex();

    // start clock after the first 2 moves
    if (index >= 2)
      m_view->run();

    m_view->changeClock(m_game->position(index)->turn());
  }
  virtual void notifyBack() { }
  virtual void notifyForward() { }
  virtual void notifyGotoFirst() { }
  virtual void notifyGotoLast() { }
  virtual void stop() {
    Agent::stop();
    m_view->stopClocks();
  }
};

class ClockUpdateAgent : public Agent {
  ChessTable* m_view;
public:
  ClockUpdateAgent(ChessTable* view)
  : m_view(view) { }

  virtual void notifyClockUpdate(int white, int black) {
    m_view->updateTime(white, black);
  }
  virtual void notifyMove(AbstractMove::Ptr, AbstractPosition::Ptr) { }
  virtual void notifyBack() { }
  virtual void notifyForward() { }
  virtual void notifyGotoFirst() { }
  virtual void notifyGotoLast() { }
};

EditGameController::EditGameController(ChessTable* view,
                                       VariantInfo* variant,
                                       AbstractPosition::Ptr startingPosition)
: Controller(view) , m_variant(variant) {

  AbstractPosition::Ptr position;
  if (!startingPosition) {
    position = m_variant->createPosition();
    position->setup();
  }
  else position = startingPosition;

  m_graphical = shared_ptr<GraphicalSystem>(new GraphicalSystem(m_view, position, m_variant));

  m_game = shared_ptr<GraphicalGame>(new GraphicalGame(m_graphical.get(),
                                     m_view->moveListTable()));
  m_game->reset(position);

  m_entity = shared_ptr<GameEntity>(new GameEntity(m_variant, m_game, m_view->board(), &m_agents));
  m_entity->setPremove(false);
  m_entity->enableEditingTools(true);

  m_graphical->setup(m_entity);
  m_game->setEntity(m_entity);


  init(AbstractPosition::Ptr());
}

void EditGameController::init(AbstractPosition::Ptr startingPosition) {
  m_players[0] = m_entity;
  m_players[1] = m_entity;

  setUserLiberties();
  if (startingPosition) {
    // TODO update to the starting position
  }

  // add user agent
  m_agents.addAgent(m_entity);

  m_clock_agent = shared_ptr<Agent>(new ClockAgent(m_view, m_game));
  m_update_agent = shared_ptr<Agent>(new ClockUpdateAgent(m_view));

  // add clock update agent
  m_agents.addAgent(m_update_agent);
}

QString EditGameController::variant() const {
  PositionPtr pos = m_game->position();
  if (pos)
    return pos->variant();
  else
    return QString();
}

void EditGameController::setPromotionType(int type) {
  m_entity->changePromotionType(type);
}

void EditGameController::onNavigation() { }

bool EditGameController::addPlayingEngine(int side, const shared_ptr<Engine>& engine) {
  Q_ASSERT(side == 0 || side == 1);
  if (m_players[side]->canDetach()) {
    shared_ptr<EngineEntity> entity(new EngineEntity(m_variant, m_game,
                                                  engine, &m_agents));
    m_agents.addAgent(entity);

    m_players[side] = entity;
    engine->setNotifier(entity);
    engine->start();
    engine->reset(side);
    // TODO: set engine state appropriately

    setUserLiberties();
  }
  else {
    std::cout << "** could not detach entity playing " << side << "**" << std::endl;
  }

  return true;
}

EntityToken EditGameController::addAnalysingEngine(const shared_ptr<Engine>& engine) {
  Q_ASSERT(engine.use_count() == 1);
  shared_ptr<EngineEntity> entity(new EngineEntity(m_variant, m_game, engine, &m_agents));
  m_agents.addAgent(entity);
  EntityToken res(m_entities.insert(entity).first);

  engine->setNotifier(entity);
  engine->start();
  engine->setBoard(m_game->position(), 0, 0);
  engine->startAnalysis();

  std::cout << "there are now " << m_entities.size() << " entities" << std::endl;

  return res;
}

void EditGameController::removeAnalysingEngine(const EntityToken& token) {
  m_entities.erase(token.iterator());
  std::cout << "there are " << m_entities.size() << " entities left" << std::endl;
}

bool EditGameController::addICSPlayer(int side, int game_number, const shared_ptr<ICSConnection>& connection) {
  Q_ASSERT(side == 0 || side == 1);
  if (m_players[side]->canDetach()) {
    shared_ptr<ICSEntity> entity(new ICSEntity(m_variant, m_game,
                                side, game_number, connection, &m_agents));

    if (entity->attach()) {
      m_agents.addAgent(entity);

      m_players[side] = entity;
      connection->setListener(game_number, entity);

      Role user_role = setUserLiberties();
      m_view->flip(user_role & PlayingBlack);

      m_agents.addAgent(m_clock_agent);
    }
    else {
      std::cout << "** could not attach ics entity **" << std::endl;
      return false;
    }
  }
  else {
    std::cout << "** could not detach entity playing " << side << "**" << std::endl;
    return false;
  }

  return true;
}

bool EditGameController::setExaminationMode(int game_number, const shared_ptr<ICSConnection>& connection) {
  std::cout << "[controller " << this << "] setting examination mode" << std::endl;
  if (m_players[0]->canDetach() &&
      m_players[1]->canDetach()) {
    shared_ptr<ExaminationEntity> entity(new ExaminationEntity(m_variant, m_game,
                                              game_number, connection, &m_agents));
    if (entity->attach()) {
      m_entity = entity;
      m_view->setEntity(entity);
      m_agents.addAgent(entity);
      m_players[0] = entity;
      m_players[1] = entity;

      connection->setListener(game_number, entity);
      m_view->flip(false);
      m_entity->setTurnTest(shared_ptr<TurnTest>(new NoTurnTest));
      return true;
    }
    else
      std::cout << "** could not attach examination entity **" << std::endl;
  }
  else
    std::cout << "** could not detach entity **" << std::endl;

  return false;
}

bool EditGameController::setObserveMode(int game_number, const shared_ptr<ICSConnection>& connection) {
  if (m_players[0]->canDetach() &&
      m_players[1]->canDetach()) {
    shared_ptr<ICSEntity> entity(new ObservingEntity(m_variant, m_game,
                                    game_number, connection, &m_agents));

    if (entity->attach()) {
      m_agents.addAgent(entity);
      m_players[0] = entity;
      m_players[1] = entity;

      connection->setListener(game_number, entity);
      m_view->flip(false);
      m_entity->setTurnTest(shared_ptr<TurnTest>(new NoTurnTest));
      return true;
    }
    else
      std::cout << "** could not attach ics entity for observe mode ** " << std::endl;
  }
  else
    std::cout << "** could not detach entity **" << std::endl;

  return false;
}

EditGameController::Role EditGameController::setUserLiberties() {
  if (m_players[0] == m_entity) {
    if (m_players[1] == m_entity) {
      m_entity->setTurnTest(
        shared_ptr<TurnTest>(new FreeTurnTest));
      m_entity->setPremove(false);
      m_entity->enableEditingTools(true);
      return static_cast<EditGameController::Role>(PlayingWhite | PlayingBlack);
    }
    else {
      m_entity->setTurnTest(
        shared_ptr<TurnTest>(new OneTurnTest(0)));
      m_entity->setPremove(true);
      m_entity->enableEditingTools(false);
      return PlayingWhite;
    }
  }
  else {
    if (m_players[1] == m_entity) {
      m_entity->setTurnTest(
        shared_ptr<TurnTest>(new OneTurnTest(1)));
      m_entity->setPremove(true);
      m_entity->enableEditingTools(false);
      return PlayingBlack;
    }
    else {
      m_entity->setTurnTest(
        shared_ptr<TurnTest>(new NoTurnTest));
      return NotPlaying;
    }
  }
}

void EditGameController::loadPGN(const PGN& pgn) {
  end();
  m_view->resetClock();
  entity()->loadPGN(pgn);
}

void EditGameController::createCtrlAction() {
  m_game->createCtrlAction();
}

void EditGameController::destroyCtrlAction() {
  m_game->destroyCtrlAction();
}

void EditGameController::detach() {
  m_entity->detach();
  m_players[0]->detach();
  m_players[1]->detach();

  foreach (shared_ptr<Entity> entity, m_entities)
    entity->detach();
}

shared_ptr<Controller> EditGameController::end() {
  // stop clocks
  if (m_clock_agent) {
    m_clock_agent->stop();
    m_clock_agent = shared_ptr<Agent>(new ClockAgent(m_view, m_game));
  }

  // return to edit game mode
  m_players[0] = m_entity;
  m_players[1] = m_entity;
  setUserLiberties();

  return Controller::end();
}
