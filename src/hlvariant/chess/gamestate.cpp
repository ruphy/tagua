/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "gamestate.h"


namespace HLVariant {
namespace Chess {

CastlingData::CastlingData()
: wk(true)
, wq(true)
, bk(true)
, bq(true) { }

CastlingData::CastlingData(bool wk, bool wq, bool bk, bool bq)
: wk(wk)
, wq(wq)
, bk(bk)
, bq(bq) { }

bool CastlingData::operator==(const CastlingData& other) const {
  return wk == other.wk &&
         wq == other.wq &&
         bk == other.bk &&
         bq == other.bq;
}


} // namespace Chess
} // namespace HLVariant

