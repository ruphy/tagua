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

void ChessWrappedTest::test_setup() {
  m_pos->setup();
  
  CPPUNIT_ASSERT(m_pos->get(Point(4, 6)));
  CPPUNIT_ASSERT(m_pos->get(Point(4, 6))->name() == "white_pawn");
  
  CPPUNIT_ASSERT(!m_pos->get(Point(5, 5)));
  
  CPPUNIT_ASSERT(m_pos->get(Point(0, 0)));
  CPPUNIT_ASSERT(m_pos->get(Point(0, 0))->name() == "black_rook");
}




