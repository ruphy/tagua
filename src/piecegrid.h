/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PIECEGRID_H
#define PIECEGRID_H

#include "grid.h"

/**
  * A specialization of Grid useful for containing pointers.
  * It is used only to contain pointers to Piece and derived classes.
  */
template <typename T>
class PointerGrid : public Grid<T*> {
public:
  using Grid<T*>::board; // why???
  PointerGrid(int sizeX, int sizeY);
  PointerGrid(const PointerGrid<T>& other);
  template <typename T1>
  PointerGrid(const PointerGrid<T1>& other);

  bool operator==(const PointerGrid<T>& other) const;

  Point find(const T& x) const {
    for (Point i = this->first(); i <= this->last(); i = this->next(i)) {
      if ((*this)[i] && *(*this)[i] == x)
        return i;
    }
    return Point::invalid();
  }

  virtual ~PointerGrid();
};


//BEGIN Implementation

template <typename T>
PointerGrid<T>::PointerGrid(int sizeX, int sizeY) : Grid<T*>(sizeX, sizeY, 0) {}

template <typename T>
PointerGrid<T>::PointerGrid(const PointerGrid<T>& other)
: Grid<T*>(other) {
  for (uint i = 0; i < board.size(); ++i) {
    T* p = other.board[i];
    if (p)
      board[i] = new T(*p);
    else
      board[i] = 0;
  }
}

template <typename T>
template <typename T1>
PointerGrid<T>::PointerGrid(const PointerGrid<T1>& other)
: Grid<T*>(other.getSize().x, other.getSize().y) {
  for (uint i = 0; i < board.size(); ++i) {
    T1* p = other.board[i];
    if (p)
      board[i] = new T(*p);
    else
      board[i] = 0;
  }
}

template <typename T>
PointerGrid<T>::~PointerGrid() {
  for (uint i = 0; i < board.size(); ++i)
    delete board[i];
}

template <typename T>
bool PointerGrid<T>::operator==(const PointerGrid<T>& other) const {
  for (uint i = 0; i < board.size(); ++i) {
    if (!board[i]) {
      if (other.board[i]) return false;
    }
    else
      if (! board[i]->equals(other.board[i])) return false;
  }
  return true;
}


//END Implementation

#endif // PIECEGRID_H
