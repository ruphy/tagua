/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GRID_H
#define GRID_H

#include <vector>
#include <iostream>

#include <QString>
#include <QPoint>
#include "point.h"

template <typename T>
class Grid {
  int sizeX, sizeY;
protected:
  std::vector<T> board;
public:
  Grid(int sizeX, int sizeY, const T& defaultValue)
  : sizeX(sizeX), sizeY(sizeY) {
    board.resize(sizeX * sizeY, defaultValue);
  }

  Grid(int sizeX, int sizeY)
  : sizeX(sizeX), sizeY(sizeY) {
    board.resize(sizeX * sizeY);
  }

  Grid(const Grid& other)
  : sizeX(other.sizeX), sizeY(other.sizeY)
  , board(other.board) { }

  Point getSize() const {
    return Point(sizeX, sizeY);
  }

  void changeSize(int x, int y) {
    sizeX = x;
    sizeY = y;
    board.resize(sizeX * sizeY);
  }

  const T& operator[](Point p) const {
    Q_ASSERT(valid(p));

    return board[p.x + p.y * sizeX];
  }
  T& operator[](Point p) {
    Q_ASSERT(valid(p));

    return board[p.x + p.y * sizeX];
  }

  const Point first() const { return Point(0,0); }
  const Point last() const { return Point(sizeX-1, sizeY-1); }

  Point next(const Point& p) const {
    if (p.x >= sizeX-1)
      return Point(0, p.y+1);
    else
      return Point(p.x+1, p.y);
  }
  bool valid(const Point& p) const {
    return (0 <= p.x) && (p.x < sizeX) && (0 <= p.y) && (p.y < sizeY);
  }

  bool operator==(const Grid& g) const {
    if(g.sizeX != sizeX || g.sizeY != sizeY)
      return false;
    for(unsigned int i=0;i<board.size();i++)
    if(board[i] != g.board[i])
      return false;
    return true;
  }
};

#endif // GRID_H
