/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GENERICPOSITION_H
#define GENERICPOSITION_H

#include "point.h"
#include "piece.h"

class GenericPosition {
public:
  virtual const ChessPiece* operator[](const Point&) const = 0;
  virtual ChessPiece* operator[](const Point&) = 0;
  
  virtual const Point first() const = 0;
  virtual const Point last() const = 0;
  virtual Point next(const Point&) const = 0;
  virtual bool valid(const Point&) const = 0;
  
  virtual void setup() = 0;
  
  virtual GenericPosition* clone() const = 0;
  virtual GenericPosition* _new() const = 0;
  //   
//   virtual void basicMovePiece(const ChessMove&) = 0;
//   virtual void basicDropPiece(ChessPiece*, const Point&) = 0;
//   virtual void removePiece(const Point&) = 0;
//   virtual void setPiece(const Point&, ChessPiece*) = 0;
//   
//   virtual void move(const ChessMove&) = 0;
   virtual ChessPiece::Color turn() const = 0;
//   virtual QString type() const = 0;
};

#endif
