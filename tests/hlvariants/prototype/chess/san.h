#ifndef HLVARIANT__CHESS__SAN_H
#define HLVARIANT__CHESS__SAN_H

#include <iostream>

#include <QRegExp>

#include "point.h"

namespace HLVariant {
namespace Chess {

class SAN {
  friend std::ostream& operator<<(std::ostream& os, const SAN& move);

  static QRegExp pattern;
  static QRegExp kingCastlingPattern;
  static QRegExp queenCastlingPattern;
  static QRegExp nonePattern;
  
  int getType(const QString& letter) const;
public:
  enum CastlingType {
      NoCastling,
      KingSide,
      QueenSide
  };
  
  SAN();

  void load(const QString&, int& offset, int ysize);
  void load(const QString&, int ysize);

  Point from, to;
  int type;
  int promotion;
  CastlingType castling;
  bool drop;

  inline bool invalid() const { return (to == Point::invalid()) && (castling == NoCastling); }
  inline bool valid() const { return !invalid(); }
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__SAN_H

