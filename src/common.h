/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef COMMON_H
#define COMMON_H

#include "export.h"
#include <math.h>

class QString;

enum ErrorCode {
  InvalidMove
};

QString prettyTypeName(const char* name);

#ifndef M_PI
  #define M_PI 3.1415926
#endif

#endif // COMMON_H

