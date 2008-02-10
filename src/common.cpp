/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "common.h"
#include <KDebug>

namespace boost {
// throw exception: used by boost
void throw_exception(std::exception const &) {
  kError() << "[bug] boost exception";
}
}

#ifdef Q_CC_GNU
  #include <typeinfo>
  #include <cxxabi.h>
#endif //Q_CC_GNU

QString prettyTypeName(const char* name) {
#ifdef Q_CC_GNU
  int s;
  const char* res = abi::__cxa_demangle(name, NULL, NULL, &s);
  if(res) {
    QString retv(res);
    free((void*)res);
    return retv;
  }
#endif //Q_CC_GNU
  return QString(name);
}
