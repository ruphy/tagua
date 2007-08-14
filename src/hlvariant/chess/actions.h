/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CHESS__ACTIONS_H
#define HLVARIANT__CHESS__ACTIONS_H

#include "actioncollection.h"

namespace HLVariant {
namespace Chess {

class Actions : public ActionCollection {
Q_OBJECT
  int m_promotion;
public:
  Actions();
  
  int promotion() const;
private Q_SLOTS:
  virtual void promote_to_queen();
  virtual void promote_to_rook();
  virtual void promote_to_bishop();
  virtual void promote_to_knight();
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__ACTIONS_H

