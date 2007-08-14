/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "examinationentity.h"
#include "game.h"
#include "icsconnection.h"
#include "positioninfo.h"
#include "pgnparser.h"
#include "hlvariant/chess/piece.h"
#include "icsapi.h"

#include <iostream>

ExaminationEntity::ExaminationEntity(const VariantPtr& variant, const boost::shared_ptr<Game>& game,
                  int game_number, const boost::shared_ptr<ICSConnection>& connection,
                  AgentGroup* group)
: UserEntity(game)
, m_variant(variant)
, m_icsapi(variant->icsAPI())
, m_game_number(game_number)
, m_connection(connection)
, m_dispatcher(group, this) {
  Q_ASSERT(m_icsapi);
}

QString ExaminationEntity::save() const {
  return m_game->pgn();
}

void ExaminationEntity::loadPGN(const PGN& pgn) {
  m_game->load(pgn);
}

AbstractMove::Ptr ExaminationEntity::testMove(const NormalUserMove&) const {
  return AbstractMove::Ptr();
}
AbstractMove::Ptr ExaminationEntity::testMove(const DropUserMove&) const {
  return AbstractMove::Ptr();
}

bool ExaminationEntity::testPremove(const NormalUserMove&) const { return false; }
bool ExaminationEntity::testPremove(const DropUserMove&) const { return false; }
void ExaminationEntity::executeMove(boost::shared_ptr<AbstractMove>) { }
void ExaminationEntity::addPremove(const NormalUserMove&) { }
void ExaminationEntity::addPremove(const DropUserMove&) { }
void ExaminationEntity::cancelPremove() { }
InteractionType ExaminationEntity::validTurn(const Point&) const { return NoAction; }
InteractionType ExaminationEntity::validTurn(int) const { return NoAction; }
bool ExaminationEntity::movable(const Point&) const { return false; }
bool ExaminationEntity::jump(const Index&) { return true; }

bool ExaminationEntity::gotoFirst() {
  m_connection->sendText("back 99999");
  return true;
}

bool ExaminationEntity::gotoLast() {
  m_connection->sendText("forward 99999");
  return true;
}

bool ExaminationEntity::goTo(const Index& index) {
  std::pair<int, int> steps = m_game->index().stepsTo(index);
  if (steps.first == 0) {
    m_connection->sendText(QString("forward %1").arg(steps.second));
    return true;
  }
  else if (steps.second == 0) {
    m_connection->sendText(QString("back %1").arg(steps.first));
    return true;
  }
  else return false;
}

bool ExaminationEntity::forward() {
  std::cout << "[entity] forward" << std::endl;
  m_connection->sendText("forward");
  return true;
}

bool ExaminationEntity::back() {
  m_connection->sendText("back");
  return true;
}

bool ExaminationEntity::undo() { return true; }
bool ExaminationEntity::redo() { return true; }
bool ExaminationEntity::truncate() { return true; }
bool ExaminationEntity::promoteVariation() { return true; }

void ExaminationEntity::notifyStyle12(const PositionInfo& style12, bool /*is_starting*/) {
  m_dispatcher.clockUpdate(style12.whiteTime, style12.blackTime);

  if (style12.index() > 0) {
    MovePtr last_move = m_icsapi->parseVerbose(style12.lastMove, style12.position);
    m_game->insert(last_move, style12.position, style12.index());
  }

  m_game->goTo(style12.index());
}
void ExaminationEntity::notifyPool(const class PoolInfo&) { /*TODO */ }
void ExaminationEntity::notifyMoveList(int, AbstractPosition::Ptr, const PGN&){ /* TODO */ }
void ExaminationEntity::notifyMove(const Index&) { }
void ExaminationEntity::notifyBack() { }
void ExaminationEntity::notifyForward() { }
void ExaminationEntity::notifyGotoFirst() { }
void ExaminationEntity::notifyGotoLast() { }

bool ExaminationEntity::attach() {
  return m_game->lastMainlineIndex() == 0;
}
