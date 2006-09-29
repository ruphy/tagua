/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENGINEENTITY_H
#define ENGINEENTITY_H

#include "entity.h"
#include "gamebasedentity.h"
#include "agent.h"
#include "enginenotifier.h"
#include "kboard.h"
#include "agentgroup.h"

class Engine;

class EngineEntity : public Entity
                   , public GameBasedEntity
                   , public Agent
                   , public EngineNotifier {
  VariantInfo* m_variant;
  boost::shared_ptr<Engine> m_engine;
  AgentGroupDispatcher m_dispatcher;
  
  void executeMove(AbstractMove::Ptr move);
public:
  boost::shared_ptr<Engine> engine() { return m_engine; }

  EngineEntity(VariantInfo* variant, const boost::shared_ptr<Game>&, 
              const boost::shared_ptr<Engine>& engine, AgentGroup* group);
  
  virtual void notifyEngineMove(const QString&);
  
  virtual bool canDetach() const { return true; }

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(AbstractMove::Ptr move, AbstractPosition::Ptr ref);
  virtual void notifyBack() { }
  virtual void notifyForward() { }
  virtual void notifyGotoFirst() { }
  virtual void notifyGotoLast() { }
};

#endif // ENGINEENTITY_H
