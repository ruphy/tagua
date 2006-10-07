/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef COMMON_H
#define COMMON_H

#include <iosfwd>
class QString;

enum ErrorCode {
  InvalidMove
};

struct null_deleter {
  void operator()(void const *) const { }
};

//class ChessPiece;
class PieceSprite;

template <typename T> class Grid;
template <typename T> class PointerGrid;
//typedef PointerGrid<ChessPiece> PieceGrid;

#define FORALL(i, collection, type) \
  for (type::iterator i = (collection).begin(); i != (collection).end(); ++i)


std::ostream &operator <<(std::ostream &os, const QString& s);

inline const char* wrap_cptr(const char* ptr) {
  return ptr ? ptr : "[NULL]";
}

QString qPrintf(const char* fmt, ...);

QString prettyTypeName(const char* name);

inline void TRAP() {
#if defined(Q_CC_GNU)
  __asm__ __volatile__("int $3\n\t");
#endif
}

#ifndef M_PI
  #define M_PI 3.14159
#endif

#endif // COMMON_H