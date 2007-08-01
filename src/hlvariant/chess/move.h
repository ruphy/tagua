/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CHESS__MOVE_H
#define HLVARIANT__CHESS__MOVE_H

#include "point.h"
#include "export.h"

namespace HLVariant {
namespace Chess {

class TAGUA_EXPORT Move {
public:
  enum Type {
    INVALID = -1,
    NORMAL,
    EN_PASSANT_CAPTURE,
    EN_PASSANT_TRIGGER,
    KING_SIDE_CASTLING,
    QUEEN_SIDE_CASTLING,
    PROMOTION
  };
private:
  Type m_type;
  
  Point m_from;
  Point m_to;
  int m_promotion;
public:
  Move();
  Move(const Point& from, const Point& to, int promotionType = -1);
  virtual ~Move();
  
  virtual Point enPassantTrigger() const;
  virtual Point captureSquare() const;
  virtual int promoteTo() const;
  virtual bool kingSideCastling() const;
  virtual bool queenSideCastling() const;
  
  virtual Point from() const;
  virtual Point to() const;
  virtual bool valid() const;
  
  virtual void setType(Type type);
  
  virtual bool operator==(const Move& move) const;
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__MOVE_H

