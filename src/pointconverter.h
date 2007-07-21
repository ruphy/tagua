/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef POINTCONVERTER_H
#define POINTCONVERTER_H

#include <iostream>

#if 0
class PointConverter {
  int m_size;
  bool m_flipped;

  inline Point flipPoint(const Point& p) const {
    if (m_flipped)
      return Point(7-p.x, 7-p.y);
    else
      return p;
  }

public:
  PointConverter(int size, bool flipped)
  : m_size(size)
  , m_flipped(flipped) { }

  inline Point toLogical(const QPoint& real) const { return flipPoint(Point(real).div(m_size)); }
  inline QPoint toReal(const Point& logical) const { return flipPoint(logical) * m_size; }

  inline void flip(bool flipped) { m_flipped = flipped; }
  inline void resize(int size) { m_size = size; }
};
#endif

class PointConverter {
protected:
  virtual ~PointConverter(){};

public:
  virtual bool  flipped() const = 0;
  virtual int   squareSize() const = 0;
  virtual Point gridSize() const = 0;

  virtual Point flipPoint(const Point& p) const {
    if (flipped())
      return gridSize()-Point(1,1)-p;
    else
      return p;
  }
  virtual Point toLogical(const QPoint& real) const {
    return flipPoint(Point(real).div( squareSize() ));
  }
  virtual QPoint toReal(const Point& logical) const {
    return flipPoint(logical) * squareSize();
  }
};


#endif // POINTCONVERTER_H
