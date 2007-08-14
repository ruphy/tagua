/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "shogiactions.h"

#include <KAction>
#include <KIcon>

namespace HLVariant {
namespace Shogi {

ShogiActions::ShogiActions()
: m_promotion(true) {
  QAction* toggle = new KAction(KIcon("bookmark"), "Promote automatically", this);
  toggle->setCheckable(true);
  toggle->setChecked(true);
  connect(toggle, SIGNAL(triggered()), this, SLOT(toggle_promotion()));
  add(toggle);
}

bool ShogiActions::promotion() const {
  return m_promotion;
}

void ShogiActions::toggle_promotion() {
  m_promotion = !m_promotion;
}

} // namespace Shogi
} // namespace HLVariant
