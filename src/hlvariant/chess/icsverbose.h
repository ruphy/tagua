/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CHESS__ICSVERBOSE_H
#define HLVARIANT__CHESS__ICSVERBOSE_H

#include "san.h"

namespace HLVariant {
namespace Chess {

class ICSVerbose {
  static QRegExp pattern;
  static QRegExp kingCastlingPattern;
  static QRegExp queenCastlingPattern;
  static QRegExp nonePattern;

public:
  ICSVerbose();
  void load(const QString&, int ysize);
  
  Point from, to;
  int type, promotion;
  SAN::CastlingType castling;
  
  inline bool invalid() const { return (from == Point::invalid() && to == Point::invalid()); }
  inline bool valid() const { return !invalid(); }
};


} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__ICSVERBOSE_H

