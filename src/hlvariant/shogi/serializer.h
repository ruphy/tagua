/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOGI__SERIALIZER_H
#define HLVARIANT__SHOGI__SERIALIZER_H

#include <QString>

namespace HLVariant {
namespace Shogi {

template <typename _LegalityCheck>
class Serializer {
public:
  enum {
    SIMPLE = 0,
    COMPACT = 1,
    DECORATED = 2
  };

  typedef _LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  typedef typename GameState::Move Move;
protected:
  int m_rep;
  
  virtual bool isAmbiguous(const Move& move, const GameState& ref) const;
  virtual QString square(const Point& p, const Point& size) const;
  virtual QChar symbol(typename Piece::Type type) const;
public:
  Serializer(int rep);
  virtual ~Serializer();
  
  QString serialize(const Move&, const GameState& ref);
  Move deserialize(const QString& str, const GameState& ref);
};

// IMPLEMENTATION

template <typename LegalityCheck>
Serializer<LegalityCheck>::Serializer(int rep)
: m_rep(rep) { }


template <typename LegalityCheck>
Serializer<LegalityCheck>::~Serializer() { }

template <typename LegalityCheck>
bool Serializer<LegalityCheck>::isAmbiguous(const Move& move, const GameState& ref) const {
  Piece piece = move.drop();
  if (piece == Piece())
    piece = ref.board().get(move.from());
    
  bool ambiguous = false;
  if (move.drop() == Piece()) {
    for (int i = 0; i < ref.board().size().x; i++) {
      for (int j = 0; j < ref.board().size().x; j++) {
        Point p(i, j);
        if (p == move.from() || ref.board().get(p) != piece)
          continue;
        Move mv(p, move.to());
        LegalityCheck check(ref);
        if (check.legal(mv)) {
          ambiguous = true;
          break;
        }
      }
    }
  }
  
  return ambiguous;
}

template <typename LegalityCheck>
QString Serializer<LegalityCheck>::square(const Point& p, const Point& size) const {
  return QString::number(size.x - p.x) + QString(p.y + 'a');
}


template <typename LegalityCheck>
QString Serializer<LegalityCheck>::serialize(const Move& move, const GameState& ref) {
  Piece piece = move.drop();
  if (piece == Piece())
    piece = ref.board().get(move.from());

  bool ambiguous = isAmbiguous(move, ref);
  
  QString res;
  if (piece.promoted())
    res += "+";

  res += symbol(piece.type());
  
  if (ambiguous) {
    res += square(move.from(), ref.board().size());
  }
  
  if (move.drop() != Piece())
    res += "*";
  else if (ref.board().get(move.to()) != Piece())
    res += "x";
  else
    res += "-";
    
  res += square(move.to(), ref.board().size());
  
  if (!piece.promoted() && 
      move.drop() == Piece() &&
      ref.promotionZone(ref.turn(), move.to())) {
    if (move.promoteTo() != -1)
      res += "+";
    else
      res += "=";
  }
  return res;
}

template <typename LegalityCheck>
QChar Serializer<LegalityCheck>::symbol(typename Piece::Type type) const {
  if (type == Piece::KNIGHT)
    return 'N';
  else
    return Piece::typeName(type)[0].toUpper();
}

template <typename LegalityCheck>
typename Serializer<LegalityCheck>::Move Serializer<LegalityCheck>::deserialize(const QString&, const GameState&) {
  return Move(); // BROKEN
}

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__SERIALIZER_H

