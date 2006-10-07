/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef VARIANTS__CHAINREACTION_H
#define VARIANTS__CHAINREACTION_H

#include "kboard.h"

class ChainReactionVariant {
private:
  static VariantInfo* static_chainreaction_variant;
public:
  static VariantInfo* info();
};

#endif //VARIANTS__CHAINREACTION_H