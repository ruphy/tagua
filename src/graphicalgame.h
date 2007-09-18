/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRAPHICALGAME_H
#define GRAPHICALGAME_H

#include <QObject>
#include <QFlags>
#include <boost/weak_ptr.hpp>
#include "game.h"
#include "movelist_notifier.h"

class GraphicalSystem;
class CtrlAction;
class UserEntity;
class GraphicalGame;
class ActionStateObserver;
namespace MoveList { class Table; }

class GraphicalGame : public QObject, public Game, public MoveList::Notifier {
Q_OBJECT
public:
  /**
    * @brief State of GUI actions associated to a game.
    */
  enum ActionStateFlag {
    UNDO = 0x1,
    REDO = 0x2,
    BEGIN = 0x4,
    BACK = 0x8,
    FORWARD = 0x10,
    END = 0x20
  };
  Q_DECLARE_FLAGS(ActionState, ActionStateFlag)
private:
  GraphicalSystem*    m_graphical;
  MoveList::Table*    m_movelist;
  bool                m_anim_sequence;
  int                 m_anim_sequence_max;

  boost::shared_ptr<CtrlAction> m_ctrl;
  boost::weak_ptr<UserEntity> m_listener_entity;
  boost::shared_ptr<ActionStateObserver> m_action_state_observer;
  ActionState m_action_state;

private Q_SLOTS:
  void settingsChanged();

public:
  GraphicalGame(GraphicalSystem* graphical, MoveList::Table* m);
  ~GraphicalGame();

  void onAddedInternal(const Index& i, bool confirm_promotion = false);
  virtual void onAdded(const Index& i);
  virtual void onRemoved(const Index& i);
  virtual void onEntryChanged(const Index& at, int propagate=1);
  virtual void onPromoteVariation(const Index& i, int v);
  virtual void onSetComment(const Index& i, const QString& s);
  virtual void onSetVComment(const Index& i, int v, const QString& s);
  virtual void onCurrentIndexChanged(const Index& old_c);
  virtual void onAvailableUndo(bool e);
  virtual void onAvailableRedo(bool e);
  virtual void onUserSelectMove(const Index& i);
  virtual void onUserSetComment(const Index& i, QString s);
  virtual void onUserSetVComment(const Index& i, int v, QString s);
  virtual void onUserClearVariations(const Index& i);
  virtual void onUserTruncate(const Index& i);
  virtual void onUserPromoteVariation(const Index& i);
  virtual void onUserRemoveVariation(const Index& i);
  virtual void onUserUndo();
  virtual void onUserRedo();
  virtual void onDetachNotifier();

  virtual void createCtrlAction();
  virtual void destroyCtrlAction();

  void setEntity(const boost::shared_ptr<UserEntity>& entity) { m_listener_entity = entity; }
  void detachEntity() { m_listener_entity.reset(); }
  void setActionStateObserver(const boost::shared_ptr<ActionStateObserver>& obs);
  void onActionStateChange();
};

Q_DECLARE_OPERATORS_FOR_FLAGS(GraphicalGame::ActionState)


/**
  * @brief An observer that is notified of changes in the ActionState flags.
  */
class ActionStateObserver {
public:
  virtual ~ActionStateObserver();
  
  virtual void notifyActionStateChange(GraphicalGame::ActionState state) = 0;
};

#endif //GRAPHICALGAME_H
