#include "chesslegalitytest.h"
#include "prototype/chess/legalitycheck.h"


CPPUNIT_TEST_SUITE_REGISTRATION(ChessLegalityTest);

void ChessLegalityTest::setUp() {
  m_state = new ChessGameState;
  m_state->setup();
  m_legality_check = new ChessLegalityCheck(*m_state);
}

void ChessLegalityTest::tearDown() {
  delete m_state;
  delete m_legality_check;
}

void ChessLegalityTest::test_simple_move() {
  ChessMove e4(Point(4, 6), Point(4, 4));
  CPPUNIT_ASSERT(m_legality_check->legal(e4));
  
  ChessMove e3(Point(4, 6), Point(4, 3));
  CPPUNIT_ASSERT(!m_legality_check->legal(e3));
  
  ChessMove Nf3(Point(6, 7), Point(5, 5));
  CPPUNIT_ASSERT(m_legality_check->legal(Nf3));
  
  ChessMove Bc4(Point(5, 7), Point(2, 4));
  CPPUNIT_ASSERT(!m_legality_check->legal(Bc4));
  
  ChessMove tmp(Point(5, 7), Point(2, 4));
  m_state->board().set(Point(4, 6), ChessPiece());
  CPPUNIT_ASSERT(m_legality_check->legal(tmp));
}



