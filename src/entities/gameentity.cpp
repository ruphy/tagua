/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "gameentity.h"
#include "premove.h"
#include "game.h"
#include "board.h"
#include "pgnparser.h"

using namespace boost;

GameEntity::GameEntity(const VariantPtr& variant, const boost::shared_ptr<Game>& game,
                       Board* chessboard, AgentGroup* group)
: UserEntity(game)
, m_variant(variant)
, m_chessboard(chessboard)
, m_dispatcher(group, this) {
}

QString GameEntity::save() const {
  return
    ((m_variant->name() == "Chess") ? QString()
     : ("[Variant \"" + m_variant->name()) + "\"]")
    + m_game->pgn();
}

void GameEntity::loadPGN(const PGN& pgn) {
  m_game->load(pgn);
}

AbstractPosition::Ptr GameEntity::doMove(AbstractMove::Ptr move) const {
  AbstractPosition::Ptr newPosition = position()->clone();
  newPosition->move(move);
  return newPosition;
}

void GameEntity::executeMove(AbstractMove::Ptr move) {
  AbstractPosition::Ptr ref = position();
  AbstractPosition::Ptr pos = doMove(move);
  m_game->add(move, pos);
  m_dispatcher.move(m_game->index());
}

void GameEntity::addPremove(const NormalUserMove& m) {
  m_premoveQueue = shared_ptr<Premove>(new Premove(m_variant->createNormalMove(m)));
}

void GameEntity::addPremove(const DropUserMove& m) {
  m_premoveQueue = shared_ptr<Premove>(new Premove(m_variant->createDropMove(m)));
}

void GameEntity::cancelPremove() {
  m_premoveQueue.reset();
  m_chessboard->cancelPremove();
}

void GameEntity::notifyMove(const Index&) {
  // the other player moved: execute premove
  if (m_premoveQueue) {
    AbstractMove::Ptr move = m_premoveQueue->execute(m_game->position());
    if (move)
      executeMove(move);
  }
  cancelPremove();
}

void GameEntity::notifyBack() { }
void GameEntity::notifyForward() { }
void GameEntity::notifyGotoFirst() { }
void GameEntity::notifyGotoLast() { }

NormalUserMove GameEntity::createMove(const Point& from, const Point& to) const {
  NormalUserMove m = UserEntity::createMove(from, to);
  m_variant->setupMove(m);
  return m;
}

AbstractMove::Ptr GameEntity::testMove(const NormalUserMove& move) const {
  AbstractMove::Ptr m = m_variant->createNormalMove(move);
  if (m && position()->testMove(m))
    return m;
  else
    return AbstractMove::Ptr();
}

AbstractMove::Ptr GameEntity::testMove(const DropUserMove& move) const {
  AbstractMove::Ptr m = m_variant->createDropMove(move);
  if (m && position()->testMove(m))
    return m;
  else
    return AbstractMove::Ptr ();
}

AbstractPiece::Ptr GameEntity::moveHint(AbstractMove::Ptr move) const {
  return position()->moveHint(move);
}

bool GameEntity::testPremove(const NormalUserMove&) const {
  return true; // TODO
}

bool GameEntity::testPremove(const DropUserMove&) const {
  return true; // TODO
}

InteractionType GameEntity::validTurn(int pool) const {
  return position()->droppable(m_turn_test, pool);
}

InteractionType GameEntity::validTurn(const Point& point) const {
  // TODO: maybe replace this call with a new API position()->owner(point) ??
  return position()->movable(m_turn_test, point);
}

bool GameEntity::movable(const Point& point) const {
  if (!m_enabled) return false;
  InteractionType action = validTurn(point);
  return m_turn_test.premove() ? action != NoAction : action == Moving;
}

bool GameEntity::oneClickMoves() const {
  return m_variant->simpleMoves();
}

bool GameEntity::gotoFirst() {
  m_game->gotoFirst();
  return true;
}

bool GameEntity::gotoLast() {
  m_game->gotoLast();
  return true;
}

bool GameEntity::goTo(const Index& index) {
  m_game->goTo(index);
  return true;
}

bool GameEntity::forward() {
  return m_game->forward();
}

bool GameEntity::back() {
  return m_game->back();
}

bool GameEntity::undo() {
  if (m_editing_tools)
    m_game->undo();
  return true;
}

bool GameEntity::redo() {
  if (m_editing_tools)
    m_game->redo();
  return true;
}

bool GameEntity::truncate() {
  if (m_editing_tools)
    m_game->truncate();
  return true;
}
bool GameEntity::promoteVariation() {
  if (m_editing_tools)
    m_game->promoteVariation();
  return true;
}

bool GameEntity::canDetach() const {
  return true;
}


