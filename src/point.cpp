/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <cmath>
#include <QDebug>
#include <QString>
#include "common.h"
#include "point.h"

Point::Point(int x, int y)
    : x(x), y(y) {

}

Point::Point(const QPoint& p)
    : x(p.x()), y(p.y()) {

}

Point::Point() {

}

Point::Point(const QString& str, int ysize) {
  x = y = -1;
  int length = str.length();
  if(length == 0)
    return;
  if(str[0].isLetter()) {
    char c = str[0].toAscii();
    if(c >= 'a' && c <= 'z')
      x = c-'a';
    else if(c >= 'A' && c <= 'Z')
      x = c-'A';
    if(length>1)
      y = ysize - str.mid(1).toInt();
  }
  else
    y = ysize - str.toInt();
}

QString Point::row(int ysize) const {
    if (y != -1)
        return QString::number(ysize - y);
    else
        return QString();
}
QString Point::numcol(int xsize) const {
    if (x != -1)
        return QString::number(xsize - x);
    else
        return QString();
}

QString Point::col() const {
    if (x != -1) {
      if(x >= 26)
        return QChar(static_cast<char>(x - 26 + 'A'));
      else
        return QChar(static_cast<char>(x + 'a'));
    }
    else
        return QString();
}
QString Point::alpharow() const {
    if (y != -1) {
      if(y >= 26)
        return QChar(static_cast<char>(y - 26 + 'A'));
      else
        return QChar(static_cast<char>(y + 'a'));
    }
    else
        return QString();
}

QString Point::toString(int ysize) const {
    return col() + row(ysize);
}

Point Point::operator+(const Point& other) const {
    return Point(x + other.x, y + other.y);
}

Point Point::operator+=(const Point& other) {
    return *this = *this + other;
}

Point Point::operator-() const {
    return Point(-x, -y);
}

Point Point::operator-(const Point& other) const {
    return Point(x - other.x, y - other.y);
}

Point Point::operator*(int n) const {
    return Point(x * n, y * n);
}

Point Point::operator/(int n) const {
    return Point(x / n, y / n);
}

Point Point::div(int n) const {
    return Point(x >= 0 ? x / n : x / n - 1,
               y >= 0 ? y / n : y / n - 1);
}

bool Point::operator==(const Point& other) const {
    return x == other.x && y == other.y;
}

bool Point::operator!=(const Point& other) const {
    return !(*this == other);
}

bool Point::operator<(const Point& other) const {
    return y < other.y || (y == other.y && x < other.x);
}

bool Point::operator<=(const Point& other) const {
    return y <= other.y || (y == other.y && x <= other.x);
}

bool Point::resembles(const Point& other) const {
    return (other.x == -1 || x == other.x) &&
            (other.y == -1 || y == other.y);
}

Point::operator QPoint() const {
    return QPoint(x,y);
}

Point Point::normalizeInfinity() const {
    return Point(
        normalizeInfinityHelper(x),
        normalizeInfinityHelper(y)
    );
}

double Point::norm() const {
    return sqrt((double)(x*x + y*y));
}

int Point::normalizeInfinityHelper(int n) const {
    if (n == 0)
        return 0;
    else
        return n > 0 ? 1 : -1;
}

QDebug operator<<(QDebug dbg, const Point& p) {
  dbg << "(" << (p.x == -1 ? QString("?") : QString::number(p.x))
      << ", " << (p.y == -1 ? QString("?") : QString::number(p.y)) << ")";
  return dbg;
}
