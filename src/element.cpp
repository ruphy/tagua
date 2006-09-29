/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "element.h"

Element::Element() { }

Element::Element(AbstractPiece::Ptr piece, SpritePtr sprite)
: m_piece(piece) //NOTE: clone?!?!??!?!?!
, m_sprite(sprite) { }

Element::operator bool() const {
  return m_piece;
}

AbstractPiece::Ptr Element::piece() const { return m_piece; }
Element::SpritePtr Element::sprite() const { return m_sprite; }


