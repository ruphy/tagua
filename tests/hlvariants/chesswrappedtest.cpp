#include "chesswrappedtest.h"
#include "hlvariant/tagua_wrapped.h"
#include "hlvariant/chess/variant.h"
#include "hlvariant/variantdata.h"

#define MOVE(x) { MovePtr _tmp = m_pos->getMove(#x);     \
                  CPPUNIT_ASSERT(_tmp);                  \
                  m_pos->move(_tmp); }

CPPUNIT_TEST_SUITE_REGISTRATION(ChessWrappedTest);

typedef HLVariant::Chess::Variant Chess;
typedef VariantData<Chess>::LegalityCheck LegalityCheck;
typedef VariantData<Chess>::MoveGenerator MoveGenerator;
typedef LegalityCheck::GameState GameState;
typedef GameState::Move ChessMove;
typedef VariantData<Chess>::Piece ChessPiece;

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

void ChessWrappedTest::test_get_move1() {
  m_pos->setup();
  
  MovePtr move = m_pos->getMove("e4");
  CPPUNIT_ASSERT_EQUAL(QString("e4"), move->SAN(m_pos));
}

void ChessWrappedTest::test_fools_mate() {
  m_pos->setup();
  
  MOVE(f3);
  MOVE(e5);
  MOVE(g4);
  MOVE(Qh4);
  
  HLVariant::WrappedPosition<Chess>* pos = 
    dynamic_cast<HLVariant::WrappedPosition<Chess>*>(m_pos.get());

  CPPUNIT_ASSERT(pos);
  
  MoveGenerator generator(pos->inner());
  CPPUNIT_ASSERT(generator.check(static_cast<ChessPiece::Color>(m_pos->turn())));
  CPPUNIT_ASSERT(generator.stalled());
}

void ChessWrappedTest::test_check() {
  m_pos->setup();
  
  MOVE(e4);
  MOVE(d5);
  MOVE(Bb5);
  
  CPPUNIT_ASSERT(!m_pos->getMove("Na6"));
  CPPUNIT_ASSERT(m_pos->getMove("Nc6"));
}



#undef MOVE
