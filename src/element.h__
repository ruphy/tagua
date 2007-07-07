/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ELEMENT_H
#define ELEMENT_H

#include "kboard.h"

class PieceSprite;

struct Element {
  typedef boost::shared_ptr<PieceSprite> SpritePtr;
private:
  AbstractPiece::Ptr m_piece;
  SpritePtr m_sprite;
public:
  Element();
  Element(AbstractPiece::Ptr piece, SpritePtr sprite);
  
  operator bool() const;
  
  AbstractPiece::Ptr piece() const;
  SpritePtr sprite() const;
};


#endif // ELEMENT_H
