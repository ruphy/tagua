#ifndef SHOGIVARIANT_H
#define SHOGIVARIANT_H

#include "chess.h"

class ShogiVariant {
private:
  static VariantInfo* static_shogi_variant;
public:
  static VariantInfo* info();
};

#endif // SHOGIVARIANT_H
