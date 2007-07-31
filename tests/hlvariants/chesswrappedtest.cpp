#include "chesswrappedtest.h"
#include "prototype/tagua_wrapped.h"
#include "prototype/chess/variant.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ChessWrappedTest);

typedef HLVariant::Chess::Variant Chess;
typedef Chess::LegalityCheck LegalityCheck;
typedef LegalityCheck::GameState GameState;
typedef GameState::Move ChessMove;

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

void ChessWrappedTest::test_move() {
  m_pos->setup();
  
  MovePtr move(new HLVariant::WrappedMove<Chess>(ChessMove(Point(4, 6), Point(4, 4))));
  CPPUNIT_ASSERT(m_pos->testMove(move));
  
  m_pos->move(move);
  
  CPPUNIT_ASSERT(!m_pos->get(Point(4, 6)));
  CPPUNIT_ASSERT(m_pos->get(Point(4, 4))->name() == "white_pawn");
}

void ChessWrappedTest::test_movable() {
  m_pos->setup();
  
  TurnTest test;
  CPPUNIT_ASSERT_EQUAL(NoAction, m_pos->movable(test, Point(4, 6)));
  CPPUNIT_ASSERT_EQUAL(NoAction, m_pos->droppable(test, 0));
  
  test.setSimplePolicy(0, true);  
  CPPUNIT_ASSERT_EQUAL(Moving, m_pos->movable(test, Point(4, 6)));
  CPPUNIT_ASSERT_EQUAL(NoAction, m_pos->movable(test, Point(5, 5)));
  CPPUNIT_ASSERT_EQUAL(NoAction, m_pos->movable(test, Point(7, 1)));
  
  CPPUNIT_ASSERT_EQUAL(Moving, m_pos->droppable(test, 0));
  CPPUNIT_ASSERT_EQUAL(NoAction, m_pos->droppable(test, 1));
  
  test.setSimplePolicy(1, true);
  CPPUNIT_ASSERT_EQUAL(Premoving, m_pos->movable(test, Point(6, 0)));
}

void ChessWrappedTest::test_san1() {
  m_pos->setup();
  
  MovePtr move(new HLVariant::WrappedMove<Chess>(ChessMove(Point(4, 6), Point(4, 4))));
  CPPUNIT_ASSERT_EQUAL(QString("e4"), move->SAN(m_pos));
}



