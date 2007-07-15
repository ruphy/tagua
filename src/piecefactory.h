#ifndef PIECEFACTORY_H
#define PIECEFACTORY_H

#include <QString>

template <typename Variant>
class PieceFactory {
public:
  static typename Variant::Piece createPiece(const QString&) { return typename Variant::Piece(); }
};

#endif // PIECEFACTORY_H

