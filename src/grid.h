/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
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
#include "pathinfo.h"

template <typename T>
class Grid {
  int sizeX, sizeY;
protected:
  std::vector<T> board;
public:

  template<typename T1> friend class Grid;

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

  template <typename T1>
  Grid(const Grid<T1>& other)
  : sizeX(other.getSize().x), sizeY(other.getSize().y) {
    board.resize(sizeX * sizeY);
    for(int i=sizeX * sizeY-1; i>=0;i--)
      board[i] = other.board[i];
  }

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

  /** \return the nth point in the grid, that is not guaranteed to be valid */
  Point nTh(int index) const { return Point(index%sizeX, index/sizeX); }

  Point first() const { return Point(0,0); }
  Point last() const { return Point(sizeX-1, sizeY-1); }

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
    if(board[i].operator!=(g.board[i]))
      return false;
    return true;
  }

  bool operator!=(const Grid& g) const {
    return !(*this == g);
  }

  PathInfo path(const Point& from, const Point& to) const {
    Point delta = to - from;
    PathInfo::Direction direction;

    if (delta.x == 0)
      direction = PathInfo::Vertical;
    else if (delta.y == 0)
      direction = PathInfo::Horizontal;
    else if (delta.x == delta.y)
      direction = PathInfo::Diagonal1;
    else if (delta.x == -delta.y)
      direction = PathInfo::Diagonal2;
    else
      direction = PathInfo::Undefined;

    int num_obs = 0;
    if (direction != PathInfo::Undefined) {
      Point step = delta.normalizeInfinity();
      Point position = from;
      while ((position += step) != to) {
        if((*this)[position].operator!())
          continue;
        num_obs++;
      }
    }

    return PathInfo(direction, num_obs);
  }
};

#endif // GRID_H
