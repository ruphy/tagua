/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            (c) 2007 Yann Dirson <ydirson@altern.org>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__TORI_SHOGI__PIECE_H
#define HLVARIANT__TORI_SHOGI__PIECE_H

#include <QString>
#include "export.h"
#include "../shogi/piece.h"

namespace HLVariant {
namespace ToriShogi {

class TAGUA_EXPORT Piece {
public:
  enum Color {
    INVALID_COLOR = -1,
    BLACK,
    WHITE
  };
  enum Type {
    PHOENIX,
    FALCON,
    CRANE,
    PHEASANT,
    LEFT_QUAIL,
    RIGHT_QUAIL,
    SWALLOW,

    // FIXME: stub names to wait for Piece refactoring
    KING,
    GOLD,
    SILVER,
    KNIGHT,
    LANCE,
    ROOK,
    BISHOP,
    PAWN,
    DRUNKEN_ELEPHANT,

    INVALID_TYPE = -1
  };
private:
  Color m_color;
  Type m_type;
  bool m_promoted;
public:
  Piece(Color color = INVALID_COLOR, Type type = INVALID_TYPE, bool promoted = false);
  virtual ~Piece();
  
  virtual Color color() const;
  virtual Type type() const;
  
  virtual QString colorName() const;
  static QString colorName(Color color);
  virtual QString typeName() const;
  static QString typeName(Type type);
  virtual QString name() const;
    
  static Color oppositeColor(Color color);

  virtual bool operator==(const Piece& other) const;
  virtual bool operator!=(const Piece& other) const;

  void setPromoted();
  bool promoted() const;
};

} // namespace ToriShogi
} // namespace HLVariant

#endif	// HLVARIANT__TORI_SHOGI__PIECE_H
