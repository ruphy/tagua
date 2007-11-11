/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CHESS__SAN_H
#define HLVARIANT__CHESS__SAN_H

#include <iostream>

#include <QRegExp>

#include "export.h"
#include "point.h"

namespace HLVariant {
namespace Chess {

class TAGUA_EXPORT SAN {
  friend std::ostream& operator<<(std::ostream& os, const SAN& move);

  static QRegExp pattern;
  static QRegExp kingCastlingPattern;
  static QRegExp queenCastlingPattern;
  static QRegExp nonePattern;
  
public:
  enum CastlingType {
      NoCastling,
      KingSide,
      QueenSide
  };
  
  SAN();

  static int getType(const QString& letter);

  void load(const QString&, int& offset, int ysize);
  void load(const QString&, int ysize);

  Point from, to;
  int type;
  int promotion;
  CastlingType castling;
  bool drop;

  inline bool invalid() const { return (to == Point::invalid()) && (castling == NoCastling); }
  inline bool valid() const { return !invalid(); }
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__SAN_H

