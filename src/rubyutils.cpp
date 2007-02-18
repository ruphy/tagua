/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "rubyutils.h"

namespace Ruby {

Point value2point(VALUE p) {
  if (NIL_P(p))
    return Point();
  else {
    int x = NUM2INT(rb_funcall(p, rb_intern("x"), 0));
    int y = NUM2INT(rb_funcall(p, rb_intern("y"), 0));
    return Point(x, y);
  }
}

VALUE point2value(const Point& p) {
  if (p.valid())
    return rb_funcall(rb_eval_string("Point"), rb_intern("new"), 2, INT2FIX(p.x), INT2FIX(p.y));
  else
    return Qnil;
}

static VALUE wrapped_call(VALUE* args) {
  VALUE rcpt = args[0];
  ID id = (ID)args[1];
  int argc = NUM2INT((VALUE)args[2]);
  VALUE* fun_args = args + 3;
  
  return rb_funcall2(rcpt, id, argc, fun_args);
}

#if 0
VALUE pcall(int* res, VALUE rcpt, ID id) {
  VALUE args[3];
  args[0] = rcpt;
  args[1] = (VALUE)id;
  args[2] = 0;
  return rb_protect((VALUE(*)(VALUE))wrapped_call, (VALUE)args, res);
}

VALUE pcall(int* res, VALUE rcpt, ID id, VALUE arg1) {
  VALUE args[4];
  args[0] = rcpt;
  args[1] = (VALUE)id;
  args[2] = 1;
  args[3] = arg1;
  return rb_protect((VALUE(*)(...))wrapped_call, (VALUE)args, res); 
}

VALUE pcall(int* res, VALUE rcpt, ID id, VALUE arg1, VALUE arg2) {
  VALUE args[5];
  args[0] = rcpt;
  args[1] = (VALUE)id;
  args[2] = 2;
  args[3] = arg1;
  args[4] = arg2;
  return rb_protect((VALUE(*)(...))wrapped_call, (VALUE)args, res);
}


VALUE pcall(int* res, VALUE rcpt, ID id, VALUE arg1, VALUE arg2, VALUE arg3) {
  VALUE args[6];
  args[0] = rcpt;
  args[1] = (VALUE)id;
  args[2] = 2;
  args[3] = arg1;
  args[4] = arg2;
  args[5] = arg3;
  return rb_protect((VALUE(*)(...))wrapped_call, (VALUE)args, res);
}
#endif

}
