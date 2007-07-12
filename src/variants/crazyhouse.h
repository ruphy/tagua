/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CRAZYHOUSE_H
#define CRAZYHOUSE_H

#include "variants/variants.h"

class CrazyhouseVariant {
private:
  static VariantInfo* static_crazyhouse_variant;

public:
  static VariantInfo* info();
};

#endif // CRAZYHOUSE_H
