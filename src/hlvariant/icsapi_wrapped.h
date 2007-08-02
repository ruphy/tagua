#ifndef HLVARIANT__ICSAPI_WRAPPED_H
#define HLVARIANT__ICSAPI_WRAPPED_H

#include "icsapi.h"
#include "tagua_wrapped.h"

namespace HLVariant {

template <typename Variant>
class WrappedICSAPI : public ICSAPI {
public:
  virtual PositionPtr createChessboard(int, bool, bool, bool, bool, const Point&);
  virtual PiecePtr createPiece(const QString& description);
};

// IMPLEMENTATION

template <typename Variant>
PositionPtr WrappedICSAPI<Variant>::createChessboard(
                          int turn,
                          bool wkCastle,
                          bool wqCastle,
                          bool bkCastle,
                          bool bqCastle,
                          const Point& enPassant) {
  return PositionPtr(new WrappedPosition<Variant>(
    typename VariantData<Variant>::GameState(
      static_cast<typename VariantData<Variant>::Piece::Color>(turn),
      wkCastle,
      wqCastle,
      bkCastle,
      bqCastle,
      enPassant)));
}

template <typename Variant>
PiecePtr WrappedICSAPI<Variant>::createPiece(const QString& description) {
  return PiecePtr(new WrappedPiece<Variant>(
    VariantData<Variant>::Piece::fromDescription(description)));
}

/**
  * Helper metafunction to create a null ICSAPI object
  * if the variant does not support ICS.
  */
template <typename Variant, bool hasICS>
struct ReturnICSAPIAux { };

template <typename Variant>
struct ReturnICSAPIAux<Variant, true> {
  static ICSAPI* apply() {
    return new WrappedICSAPI<Variant>();
  }
};

template <typename Variant>
struct ReturnICSAPIAux<Variant, false> {
  static ICSAPI* apply() { return 0; }
};

template <typename Variant>
struct ReturnICSAPI 
: public ReturnICSAPIAux<Variant, Variant::hasICS> { };



}

#endif // HLVARIANT__ICSAPI_WRAPPED_H

