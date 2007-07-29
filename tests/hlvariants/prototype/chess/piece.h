#ifndef HLVARIANT__CHESS__PIECE_H
#define HLVARIANT__CHESS__PIECE_H

#include <QString>

namespace HLVariant {
namespace Chess {

class Piece {
public:
  enum Color {
    INVALID_COLOR = -1,
    WHITE,
    BLACK
  };
  enum Type {
    INVALID_TYPE = -1,
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    QUEEN,
    KING
  };
private:
  Color m_color;
  Type m_type;
public:
  Piece(Color color = INVALID_COLOR, Type type = INVALID_TYPE);
  
  Color color() const;
  Type type() const;
  
  QString colorName() const;
  static QString colorName(Color color);
  QString typeName() const;
  static QString typeName(Type type);
  QString name() const;
  
  static Color oppositeColor(Color color);
  
  bool operator==(const Piece& other) const;
  bool operator!=(const Piece& other) const;
};

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__PIECE_H

