/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

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
  virtual MovePtr parseVerbose(const QString& str, const PositionPtr& ref);
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

template <typename Variant>
MovePtr WrappedICSAPI<Variant>::parseVerbose(const QString& str, const PositionPtr& _ref) {
  if (!_ref)
    return MovePtr();
    
  WrappedPosition<Variant>* ref = dynamic_cast<WrappedPosition<Variant>*>(_ref.get());
  if (ref) {
    typename VariantData<Variant>::Serializer serializer("ics-verbose");
    return MovePtr(new WrappedMove<Variant>(
      serializer.deserialize(str, ref->inner())));
  }
  else {
    MISMATCH(*_ref.get(), WrappedPosition<Variant>);
    return MovePtr();
  }
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

