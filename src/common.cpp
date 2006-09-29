/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

// throw exception: used by boost

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <iostream>
#include <QString>
#include "common.h"

namespace boost {

void throw_exception(std::exception const &) {
  std::cout << "[bug] boost exception" << std::endl;
}

}

std::ostream &operator <<(std::ostream &os, const QString& s) {
    os << s.toAscii().constData();
    return os;
}

#ifdef Q_CC_GNU
  void __attribute__ ((used))
#else
  void
#endif
pstr(const QString& s) {
  std::cout << "\"" << s << "\"" << std::endl;
}

QString qPrintf(const char* fmt, ...) {
  va_list ap;

  va_start(ap, fmt);
  int l = vsnprintf(NULL, 0, fmt, ap)+1;
  va_end(ap);

  char *str = (char*)alloca(l);
  //char *str = (char*)malloc(l);
  va_start(ap, fmt);
  vsnprintf(str, l, fmt, ap);
  va_end(ap);

  QString retv(str);

  //free(str);
  return retv;
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
