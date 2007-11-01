/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__TORI_SHOGI__SERIALIZER_H
#define HLVARIANT__TORI_SHOGI__SERIALIZER_H

#include "../shogi/serializer.h"
#include "piece.h"

// FIXME:
// - needs a deserializer and getType() API able to deal with 2-char type

namespace HLVariant {
namespace ToriShogi {

template <typename _LegalityCheck>
  class Serializer: public HLVariant::Shogi::Serializer<_LegalityCheck> {
public:
  typedef HLVariant::Shogi::Serializer<_LegalityCheck> Base;
  typedef _LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  Serializer(const QString& rep);
protected:
  virtual QString symbol(const Piece& piece) const;
};

template <typename LegalityCheck>
Serializer<LegalityCheck>::Serializer(const QString& rep)
: Base(rep) { }

template <typename LegalityCheck>
QString Serializer<LegalityCheck>::symbol(const Piece& piece) const {
  if (Base::m_rep == "decorated") {
    QString res = "{";
    if (piece.promoted())
      res += "p_";
    res += piece.typeName();
    return res + "}";
  } else {
    switch (piece.type()) {
    case Piece::PHOENIX:
      return "Ph";
    case Piece::FALCON:
      return "Fa";
    case Piece::CRANE:
      return "Cr";
    case Piece::PHEASANT:
      return "Pt";
    case Piece::LEFT_QUAIL:
      return "LQ";
    case Piece::RIGHT_QUAIL:
      return "RQ";
    case Piece::SWALLOW:
      return "Sw";
    default:
      return "??";
    }
  }
}

} // namespace ToriShogi
} // namespace HLVariant

#endif // HLVARIANT__TORI_SHOGI__SERIALIZER_H

