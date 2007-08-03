#include "shogideserializationtest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ShogiDeserializationTest);

void ShogiDeserializationTest::setUp() {
  m_state = new ShogiGameState;
}

void ShogiDeserializationTest::tearDown() {
  delete m_state;
}

void ShogiDeserializationTest::regression_P_drop_2c() {
  ShogiSerializer s(ShogiSerializer::SIMPLE);
  
  m_state->pools().pool(ShogiPiece::BLACK).add(ShogiPiece::PAWN);
  
  ShogiMove m = s.deserialize("P*2c", *m_state);
  CPPUNIT_ASSERT(m.drop() == ShogiPiece(ShogiPiece::BLACK, ShogiPiece::PAWN));
  CPPUNIT_ASSERT(m.to() == Point(7, 2));
  
  m_state->setup();
  m_state->board().set(Point(7, 2), ShogiPiece());
  m_state->board().set(Point(7, 6), ShogiPiece());
  
  m = s.deserialize("P*2c", *m_state);
  CPPUNIT_ASSERT(m.drop() == ShogiPiece(ShogiPiece::BLACK, ShogiPiece::PAWN));
  CPPUNIT_ASSERT(m.to() == Point(7, 2));  
}

