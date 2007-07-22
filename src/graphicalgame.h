/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRAPHICALGAME_H
#define GRAPHICALGAME_H

#include <QObject>
#include <boost/weak_ptr.hpp>
#include "game.h"
#include "movelist_notifier.h"

class GraphicalSystem;
class CtrlAction;
class UserEntity;
class GraphicalGame;
namespace MoveList { class Table; }

class GraphicalGame : public QObject, public Game, public MoveList::Notifier {
Q_OBJECT

private:
  GraphicalSystem*    m_graphical;
  MoveList::Table*    m_movelist;
  bool                m_anim_sequence;
  int                 m_anim_sequence_max;

  boost::shared_ptr<CtrlAction> m_ctrl;
  boost::weak_ptr<UserEntity> m_listener_entity;

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
};

#endif //GRAPHICALGAME_H
