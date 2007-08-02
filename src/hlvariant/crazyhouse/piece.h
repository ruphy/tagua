/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__CRAZYHOUSE__PIECE_H
#define HLVARIANT__CRAZYHOUSE__PIECE_H

#include "../chess/piece.h"

namespace HLVariant {
namespace Crazyhouse {

class Piece : public Chess::Piece {
  bool m_promoted;
public:
  Piece(Color color = INVALID_COLOR, Type type = INVALID_TYPE);
  
  virtual bool operator==(const Piece& other) const;
  virtual void setPromoted();
  virtual Type actualType() const;
  
  static Piece fromDescription(const QString& description);
};

} // namespace Crazyhouse
} // namespace HLVariant

#endif // HLVARIANT__CRAZYHOUSE__PIECE_H

