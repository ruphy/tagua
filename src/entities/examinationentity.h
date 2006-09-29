/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef EXAMINATIONENTITY_H
#define EXAMINATIONENTITY_H

#include "userentity.h"
#include "kboard.h"
#include "icslistener.h"
#include "agent.h"
#include "gamebasedentity.h"
#include "agentgroup.h"

class ICSConnection;

class ExaminationEntity : public UserEntity
                        , public GameBasedEntity
                        , public ICSListener {
  VariantInfo* m_variant;
  int m_game_number;
  boost::shared_ptr<ICSConnection> m_connection;
  AgentGroupDispatcher m_dispatcher;
public:
  ExaminationEntity(VariantInfo*, const boost::shared_ptr<Game>&,
                    int, const boost::shared_ptr<ICSConnection>&, AgentGroup*);

  virtual QString save() const;
  virtual void loadPGN(const QString&);

  virtual AbstractMove::Ptr testMove(const NormalUserMove&) const;
  virtual AbstractMove::Ptr testMove(const DropUserMove&) const;
  virtual bool testPremove(const NormalUserMove&) const;
  virtual bool testPremove(const DropUserMove&) const;
  virtual void executeMove(boost::shared_ptr<AbstractMove>);
  virtual void addPremove(const NormalUserMove&);
  virtual void addPremove(const DropUserMove&);
  virtual void cancelPremove();
  virtual Entity::Action validTurn(const Point&) const;
  virtual Entity::Action validTurn(int) const;
  virtual bool movable(const Point&) const;
  virtual bool jump(const Index&);
  virtual bool gotoFirst();
  virtual bool gotoLast();
  virtual bool goTo(const Index&);
  virtual bool forward();
  virtual bool back();
  virtual bool undo();
  virtual bool redo();
  virtual bool truncate();
  virtual bool promoteVariation();

  virtual void notifyStyle12(const PositionInfo&, bool is_starting);
  virtual void notifyPool(const class PoolInfo&);
  virtual void notifyMoveList(int, AbstractPosition::Ptr, const PGN&);

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(AbstractMove::Ptr, AbstractPosition::Ptr);
  virtual void notifyBack();
  virtual void notifyForward();
  virtual void notifyGotoFirst();
  virtual void notifyGotoLast();

  virtual bool attach();
};

#endif // EXAMINATIONENTITY_H
