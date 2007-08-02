#ifndef HLVARIANT__SHOGI__SHOGIBAN_H
#define HLVARIANT__SHOGI__SHOGIBAN_H

#include "../customboard.h"

namespace HLVariant {
namespace Shogi {

template <int size_x, int size_y, typename _Piece>
class ShogiBan : public CustomBoard<size_x, size_y, _Piece> {
public:
  typedef _Piece Piece;
  typedef CustomBoard<size_x, size_y, _Piece> Base;
  
  QStringList borderCoords() const;
};

// IMPLEMENTATION

template <int size_x, int size_y, typename Piece>
QStringList ShogiBan<size_x, size_y, Piece>::borderCoords() const {
  QStringList retv;
  for (int i = this->size().y; i > 0; i--)
    retv += QString::number(i);
    
  switch (this->size().x) {
  default:
  case 9:
    retv << QChar(0x4e5d);
  case 8:
    retv << QChar(0x516b);
  case 7:
    retv << QChar(0x4e03);
  case 6:
    retv << QChar(0x516d);
  case 5:
    retv << QChar(0x4e94);
  case 4:
    retv << QChar(0x56db);
  case 3:
    retv << QChar(0x4e09);
  case 2:
    retv << QChar(0x4e8c);
  case 1:
    retv << QChar(0x4e00);
  }
  
  return retv + retv;
}

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__SHOGIBAN_H

