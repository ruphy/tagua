#include "variant.h"

#include <functional>
#include <boost/bind.hpp>

#include <QActionGroup>
#include <KAction>

#include "../tagua_wrapped.h"
#include "actioncollection.h"

namespace HLVariant {
namespace Chess {

const char* Variant::m_name = "Chess";
const char* Variant::m_theme_proxy = "Chess";

Variant::Variant()
: m_promotion(GameState::Board::Piece::QUEEN) { }

QAction* createAction(QActionGroup* group, const KIcon& icon, const QString& text) {
  QAction* action = new KAction(icon, text, group);
  action->setCheckable(true);
  group->addAction(action);
  return action;
}

void Variant::setupActions(ActionCollection* actions) {
  QActionGroup* group = new QActionGroup(actions);
  
  QAction* queen = createAction(group, KIcon("promoteQueen"), "Promote to queen");
  queen->setChecked(true);
  actions->add(queen, boost::bind(std::mem_fun(&Variant::promote_to_queen), this));
  
  QAction* rook = createAction(group, KIcon("promoteRook"), "Promote to rook");
  actions->add(rook, boost::bind(std::mem_fun(&Variant::promote_to_rook), this));
  
  QAction* bishop = createAction(group, KIcon("promoteBishop"), "Promote to bishop");
  actions->add(bishop, boost::bind(std::mem_fun(&Variant::promote_to_bishop), this));
  
  QAction* knight = createAction(group, KIcon("promoteKnight"), "Promote to knight");
  actions->add(knight, boost::bind(std::mem_fun(&Variant::promote_to_knight), this));
}

void Variant::promote_to_queen() {
  m_promotion = GameState::Board::Piece::QUEEN;
}

void Variant::promote_to_rook() {
  m_promotion = GameState::Board::Piece::ROOK;
}

void Variant::promote_to_bishop() {
  m_promotion = GameState::Board::Piece::BISHOP;
}

void Variant::promote_to_knight() {
  m_promotion = GameState::Board::Piece::KNIGHT;
}

} // namespace Chess
} // namespace HLVariant


