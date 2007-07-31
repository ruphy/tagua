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

