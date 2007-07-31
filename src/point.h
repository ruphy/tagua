/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef POINT_H
#define POINT_H

#include <QPoint>
#include <iostream>

#include "export.h"

struct TAGUA_EXPORT Point {
  static Point invalid() { return Point(-1, -1); }
  bool valid() const { return x != -1 && y != -1; }

  int x, y;
  Point(int x, int y);
  Point();

  QString row(int ysize) const;
  QString col() const;

  explicit Point(const QString& str, int ysize);
  explicit Point(const QPoint&);

  QString toString(int ysize) const;

  Point operator+(const Point& other) const;
  Point operator+=(const Point& other);
  Point operator-() const;
  Point operator-(const Point& other) const;
  Point operator*(int n) const;
  Point operator/(int n) const;
  Point div(int n) const;
  bool operator==(const Point& other) const;
  bool operator!=(const Point& other) const;
  bool operator<(const Point& other) const;
  bool operator<=(const Point& other) const;

  // true if for each coordinate of other, either it
  // is -1, or it is equal to the corresponding
  // coordinate of this
  bool resembles(const Point& other) const;

  operator QPoint() const;

  Point normalizeInfinity() const;
  double norm() const;

private:
  int normalizeInfinityHelper(int n) const;
};

TAGUA_EXPORT std::ostream& operator<<(std::ostream&, const Point&);

#endif // POINT_H
