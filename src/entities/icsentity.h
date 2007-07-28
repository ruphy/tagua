/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSENTITY_H
#define ICSENTITY_H

#include "entity.h"
#include "agent.h"
#include "agentgroup.h"
#include "icslistener.h"
#include "tagua.h"

class ICSConnection;

class ICSEntity : public Entity
                , public Agent
                , public ICSListener {
protected:
  VariantInfo* m_variant;
  boost::shared_ptr<ICSConnection> m_connection;
  int m_side;
  int m_game_number;
  bool m_editing_mode;
  AgentGroupDispatcher m_dispatcher;

  void updateGame(const Index& index, AbstractMove::Ptr icsMove,
                                 AbstractPosition::Ptr icsPos);

  enum UpdateType {
    MoveForward,
    MoveBack,
    Forward,
    Back,
    NoMove,
    NonComparableIndexes
  };

  UpdateType getUpdate(const Index& index);
public:
  ICSEntity(VariantInfo*, const boost::shared_ptr<Game>&,
           int, int,
           const boost::shared_ptr<ICSConnection>&, AgentGroup*);

  virtual void executeMove(AbstractMove::Ptr);
  virtual void notifyStyle12(const PositionInfo&, bool is_starting);
  virtual void notifyPool(const class PoolInfo&);
  virtual void notifyMoveList(int, AbstractPosition::Ptr, const PGN&);

  virtual AbstractPosition::Ptr position() const;

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(const Index&);
  virtual void notifyBack() { }
  virtual void notifyForward() { }
  virtual void notifyGotoFirst() { }
  virtual void notifyGotoLast() { }

  void requestMoves();

  virtual bool canDetach() const;
  virtual bool attach();

  bool canEdit() const;
  bool canEdit(const Index& index) const;
  void setupTurnTest(TurnTest& test) const;
};

class ObservingEntity : public ICSEntity {
protected:
  bool m_attached;
public:
  ObservingEntity(VariantInfo*, const boost::shared_ptr<Game>&,
                  int gameNumber, const boost::shared_ptr<ICSConnection>&, AgentGroup*);
  ~ObservingEntity();

  virtual void detach();
};

#endif // ICSENTITY_H
