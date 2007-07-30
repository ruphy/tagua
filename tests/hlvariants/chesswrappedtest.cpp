#include "chesswrappedtest.h"
#include "prototype/tagua_wrapped.h"
#include "prototype/chess/variant.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ChessWrappedTest);

typedef HLVariant::Chess::Variant Chess;
typedef Chess::LegalityCheck LegalityCheck;
typedef LegalityCheck::GameState GameState;

void ChessWrappedTest::setUp() {
  m_pos = PositionPtr(new HLVariant::WrappedPosition<Chess>(GameState()));
}

void ChessWrappedTest::tearDown() {
  m_pos.reset();
}




