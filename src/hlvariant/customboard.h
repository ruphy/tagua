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
