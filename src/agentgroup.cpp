/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "agentgroup.h"

using namespace boost;

void AgentGroup::addAgent(const AgentPtr& agent) {
  m_agents.insert(agent);
}

#define FORWARD(method, args)                                       \
  for (weak_set<Agent>::iterator i = m_agents.begin();              \
       i != m_agents.end();                                         \
       ++i) {                                                       \
    if (i.get() != source) {                                        \
      i->method args;                                               \
    }                                                               \
  }                                                                 \
  std::cout << std::endl;
  
void AgentGroup::clockUpdate(Agent* source, int white, int black) {
  FORWARD(notifyClockUpdate, (white, black))
}

void AgentGroup::move(Agent* source, const Index& index) {
  FORWARD(notifyMove, (index))
}

void AgentGroup::back(Agent* source) {
  FORWARD(notifyBack, ())
}

void AgentGroup::forward(Agent* source) {
  FORWARD(notifyForward, ())
}

void AgentGroup::gotoFirst(Agent* source) {
  FORWARD(notifyGotoFirst, ())
}

void AgentGroup::gotoLast(Agent* source) {
  FORWARD(notifyGotoLast, ())
}
#undef FORWARD




AgentGroupDispatcher::AgentGroupDispatcher(AgentGroup* group, Agent* agent)
: m_group(group)
, m_agent(agent) { }

void AgentGroupDispatcher::clockUpdate(int white, int black) {
  m_group->clockUpdate(m_agent, white, black);
}

bool AgentGroupDispatcher::move(const Index& index) {
  m_group->move(m_agent, index);
  return true;
}

bool AgentGroupDispatcher::back() {
  m_group->back(m_agent);
  return true;
}

bool AgentGroupDispatcher::forward() {
  m_group->forward(m_agent);
  return true;
}

bool AgentGroupDispatcher::gotoFirst() {
  m_group->gotoFirst(m_agent);
  return true;
}

bool AgentGroupDispatcher::gotoLast() {
  m_group->gotoLast(m_agent);
  return true;
}

