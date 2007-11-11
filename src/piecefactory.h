/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECEFACTORY_H
#define PIECEFACTORY_H

#include <QString>

template <typename Variant>
class PieceFactory {
public:
  static typename Variant::Piece createPiece(const QString&) { return typename Variant::Piece(); }
};

#endif // PIECEFACTORY_H

