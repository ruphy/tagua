/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOGI__SHOGIACTIONS_H
#define HLVARIANT__SHOGI__SHOGIACTIONS_H

#include "actioncollection.h"

namespace HLVariant {
namespace Shogi {

class ShogiActions : public ActionCollection {
Q_OBJECT
  bool m_promotion;
public:
  ShogiActions();
  
  bool promotion() const;
private Q_SLOTS:
  void toggle_promotion();
};

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__SHOGIACTIONS_H
