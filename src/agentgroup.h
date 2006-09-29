/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef AGENTGROUP_H
#define AGENTGROUP_H

#include <boost/shared_ptr.hpp>
#include <vector>
#include "weakset.h"
#include "kboard.h"
#include "agent.h"

class AgentGroup {
  typedef boost::shared_ptr<Agent> AgentPtr;
  typedef std::vector<AgentPtr> AgentList;

  weak_set<Agent> m_agents;
public:
  void addAgent(const AgentPtr& agent);

  void clockUpdate(Agent* source, int, int);
  void move(Agent* source, AbstractMove::Ptr, AbstractPosition::Ptr);
  void back(Agent* source);
  void forward(Agent* source);
  void gotoFirst(Agent* source);
  void gotoLast(Agent* source);
};

class AgentGroupDispatcher {
  AgentGroup* m_group;
  Agent* m_agent;
public:
  AgentGroupDispatcher(AgentGroup* group, Agent* agent);
  virtual ~AgentGroupDispatcher(){}

  virtual void clockUpdate(int, int);
  virtual bool move(AbstractMove::Ptr, AbstractPosition::Ptr);
  virtual bool back();
  virtual bool forward();
  virtual bool gotoFirst();
  virtual bool gotoLast();
};

#endif // AGENTGROUP_H
