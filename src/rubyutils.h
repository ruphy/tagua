/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

extern "C" {
#include <ruby.h>
}
#include "point.h"

namespace Ruby {
  
Point value2point(VALUE);
VALUE point2value(const Point&);

#if 0
VALUE pcall(int* res, VALUE rcpt, ID id);
VALUE pcall(int* res, VALUE rcpt, ID id, VALUE arg1);
VALUE pcall(int* res, VALUE rcpt, ID id, VALUE arg1, VALUE arg2);
VALUE pcall(int* res, VALUE rcpt, ID id, VALUE arg1, VALUE arg2, VALUE arg3);
#endif

}
