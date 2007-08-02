/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CUSTOMBOARD_H
#define HLVARIANT__CUSTOMBOARD_H

#include "board.h"

namespace HLVariant {

template <int size_x, int size_y, typename _Piece>
class CustomBoard : public Board<_Piece> {
  typedef Board<_Piece> Base;
public:
  typedef _Piece Piece;

  CustomBoard();
  
  CustomBoard(const CustomBoard<size_x, size_y, Piece>& other);
};

// IMPLEMENTATION

template <int size_x, int size_y, typename Piece>
CustomBoard<size_x, size_y, Piece>::CustomBoard()
: Base(Point(size_x, size_y)) { }

template <int size_x, int size_y, typename Piece>
CustomBoard<size_x, size_y, Piece>::CustomBoard(const CustomBoard<size_x, size_y, Piece>& other)
: Base(other) { }


}

#endif // HLVARIANT__BOARD_H
