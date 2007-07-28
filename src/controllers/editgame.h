/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef EDITGAMECONTROLLER_H
#define EDITGAMECONTROLLER_H

#include <set>
#include "abstract.h"
#include "agentgroup.h"
#include "tagua.h"

class Entity;
class Engine;
class Agent;
class ICSConnection;
class UserEntity;
class GraphicalGame;
class GraphicalSystem;


/**
  * @brief A controller to edit or play games.
  *
  * EditGameController is used to play games, either locally or
  * on an ICS server. A pair of entities @code m_players, representing
  * the two players, is mantained as well as a set of additional entities,
  * @code m_entities.
  * If an entity needs to be notified about how the game progresses, one
  * should register it as an agent, adding it to @code m_agents.
  */
class EditGameController : public Controller {
  void init(AbstractPosition::Ptr);
  boost::shared_ptr<Agent> m_clock_agent;
  boost::shared_ptr<Agent> m_update_agent;
protected:
  virtual void onNavigation();

  boost::shared_ptr<UserEntity> m_entity;
  boost::shared_ptr<GraphicalSystem> m_graphical;
  AgentGroup m_agents;
  VariantInfo* m_variant;
  boost::shared_ptr<GraphicalGame> m_game;
  virtual boost::shared_ptr<UserEntity> entity() const { return m_entity; }
  boost::shared_ptr<Entity> m_players[2];
  std::set<boost::shared_ptr<Entity> > m_entities;
public:
  EditGameController(ChessTable*, VariantInfo*,
    AbstractPosition::Ptr startingPos = AbstractPosition::Ptr());
  virtual void setPromotionType(int);

  virtual QString variant() const;
  bool addPlayingEngine(int side, const boost::shared_ptr<Engine>& engine);
  EntityToken addAnalysingEngine(const boost::shared_ptr<Engine>& engine);
  void removeAnalysingEngine(const EntityToken& token);
  bool addICSPlayer(int side, int game_number, const boost::shared_ptr<ICSConnection>& connection);
  bool setExaminationMode(int game_number, const boost::shared_ptr<ICSConnection>& connection);
  bool setObserveMode(int game_number, const boost::shared_ptr<ICSConnection>& connection);

  virtual void loadPGN(const PGN&);

  virtual void createCtrlAction();
  virtual void destroyCtrlAction();

  virtual boost::shared_ptr<Controller> end();
  virtual void detach();
};


#endif
