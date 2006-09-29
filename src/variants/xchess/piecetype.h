/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECETYPE_H
#define PIECETYPE_H

enum PieceColor {
  INVALID_COLOR = -1,
  WHITE = 0,
  BLACK = 1
};

enum PieceType {
  INVALID_TYPE = -1,
  PAWN,
  KNIGHT,
  BISHOP,
  ROOK,
  QUEEN,
  KING
};

#endif // PIECETYPE_H
