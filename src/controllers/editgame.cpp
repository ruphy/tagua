/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "editgame.h"
#include <KDebug>
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
#include "actioncollection.h"
#include "ui.h"

using namespace boost;

class ClockAgent : public Agent {
  ChessTable* m_view;
  shared_ptr<Game> m_game;
public:
  ClockAgent(ChessTable* view, const shared_ptr<Game>& game)
  : m_view(view)
  , m_game(game) { }

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(const Index& index) {
    // start clock after the first 2 moves
    if (index.totalNumMoves() >= 2)
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
  virtual void notifyMove(const Index&) { }
  virtual void notifyBack() { }
  virtual void notifyForward() { }
  virtual void notifyGotoFirst() { }
  virtual void notifyGotoLast() { }
};

EditGameController::EditGameController(ChessTable* view,
                                       const VariantPtr& variant,
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
  m_entity->enableEditingTools(true);

  m_graphical->setup(m_entity);
  m_game->setEntity(m_entity);


  init(AbstractPosition::Ptr());
}

EditGameController::~EditGameController() { }

void EditGameController::init(AbstractPosition::Ptr startingPosition) {
  m_players[0] = m_entity;
  m_players[1] = m_entity;
  m_entity->turnTest().setSimplePolicy(0, true);
  m_entity->turnTest().setSimplePolicy(1, true);  
  
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

ActionCollection* EditGameController::variantActions() const {
  return m_variant->actions();
}

QString EditGameController::variant() const {
  PositionPtr pos = m_game->position();
  if (pos)
    return pos->variant();
  else
    return QString();
}

void EditGameController::onNavigation() { }

EntityToken EditGameController::addPlayingEngine(int side, const shared_ptr<Engine>& engine) {
  Q_ASSERT(side == 0 || side == 1);
  if (!engine)
    return EntityToken();
    
  if (m_players[side]->canDetach()) {
    shared_ptr<EngineEntity> entity(new EngineEntity(m_variant, m_game, side,
                                                  engine, &m_agents));
    entity->setup();
    
    m_agents.addAgent(entity);
    m_players[side] = entity;
    
    // the user cannot move the entity's pieces
    m_entity->turnTest().setSimplePolicy(side, false);
    
    return EntityToken(entity);
  }
  else {
    kDebug() << "** could not detach entity playing " << side << "**";
  }

  return EntityToken();
}

// EntityToken EditGameController::addAnalysingEngine(const shared_ptr<Engine>& engine) {
//   Q_ASSERT(engine.use_count() == 1);
//   shared_ptr<EngineEntity> entity(new EngineEntity(m_variant, m_game, -1, engine, &m_agents));
//   m_agents.addAgent(entity);
//   EntityToken res(m_entities.insert(entity).first);
// 
//   engine->setNotifier(entity);
//   engine->start();
//   engine->setBoard(m_game->position(), 0, 0);
//   engine->startAnalysis();
// 
//   return res;
// }

void EditGameController::removeEntity(const EntityToken& token) {
  shared_ptr<Entity> entity = token.entity();
  if (entity) {
    // check players
    for (int i = 0; i < 2; i++) {
      if (entity == m_players[i]) {
        m_players[i] = m_entity;
        m_entity->turnTest().setSimplePolicy(i, true);
      }
    }
    
    // check other entities
    std::set<boost::shared_ptr<Entity> >::iterator it = m_entities.find(entity);
    if (it != m_entities.end()) {
      m_entities.erase(it);
    }
  }
  kDebug() << "there are " << m_entities.size() << " entities left";
}

bool EditGameController::addICSPlayer(int side, int game_number, const shared_ptr<ICSConnection>& connection) {
  Q_ASSERT(side == 0 || side == 1);
  if (m_players[side]->canDetach()) {
    shared_ptr<ICSEntity> entity(new ICSEntity(m_variant, m_game,
                                side, game_number, connection, &m_agents));

    if (entity->attach()) {
      kDebug() << "added ICS agent " << entity.get();
      m_agents.addAgent(entity);

      m_players[side] = entity;
      connection->setListener(game_number, entity);

      entity->setupTurnTest(m_entity->turnTest());
      m_view->flip(m_players[1] == m_entity); // flip if we're black!

      m_agents.addAgent(m_clock_agent);
    }
    else {
      kDebug() << "** could not attach ics entity **";
      return false;
    }
  }
  else {
    kDebug() << "** could not detach entity playing " << side << "**";
    return false;
  }

  return true;
}

bool EditGameController::setExaminationMode(int game_number, const shared_ptr<ICSConnection>& connection) {
  kDebug() << "[controller " << this << "] setting examination mode";
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
      m_entity->turnTest().clear();
      
      return true;
    }
    else
      kDebug() << "** could not attach examination entity **";
  }
  else
    kDebug() << "** could not detach entity **";

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
      m_entity->turnTest().clear();
      return true;
    }
    else
      kDebug() << "** could not attach ics entity for observe mode ** ";
  }
  else
    kDebug() << "** could not detach entity **";

  return false;
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
  m_entity->turnTest().setSimplePolicy(0, true);
  m_entity->turnTest().setSimplePolicy(1, true);

  return Controller::end();
}

void EditGameController::reloadSettings() {
  m_graphical->settingsChanged();
}

void EditGameController::setUI(UI& ui) {
  m_game->setActionStateObserver(ui.createActionStateObserver(shared_from_this()));
  m_game->onActionStateChange();
}

void EditGameController::activate() {
  m_game->onActionStateChange();
}
