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

void ChessLegalityTest::test_movements() {
  ChessMove e4(Point(4, 6), Point(4, 4));
  CPPUNIT_ASSERT_EQUAL(ChessMove::EN_PASSANT_TRIGGER,
    m_legality_check->getMoveType(
      m_state->board().get(Point(4, 6)), e4));
    
  ChessMove crazy(Point(0, 0), Point(7, 4));
  CPPUNIT_ASSERT_EQUAL(ChessMove::INVALID,
    m_legality_check->getMoveType(
      m_state->board().get(Point(0, 0)), crazy));
}

void ChessLegalityTest::test_pseudolegal() {
  ChessMove e4(Point(4, 6), Point(4, 4));
  CPPUNIT_ASSERT(m_legality_check->pseudolegal(e4));
  
  ChessMove e5(Point(4, 6), Point(4, 3));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(e5));
  
  ChessMove Nf3(Point(6, 7), Point(5, 5));
  CPPUNIT_ASSERT(m_legality_check->pseudolegal(Nf3));
  
  ChessMove Bc4(Point(5, 7), Point(2, 4));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(Bc4));
  
  // black moves
  m_state->switchTurn();
  
  ChessMove crazy(Point(0, 0), Point(4, 7));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(crazy));
  
  ChessMove e5_(Point(4, 1), Point(4, 3));
  CPPUNIT_ASSERT(m_legality_check->pseudolegal(e5_));
}

void ChessLegalityTest::test_simple_move() {
  ChessMove e4(Point(4, 6), Point(4, 4));
  CPPUNIT_ASSERT(m_legality_check->legal(e4));
  
  ChessMove e5(Point(4, 6), Point(4, 3));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(e5));
  
  ChessMove Nf3(Point(6, 7), Point(5, 5));
  CPPUNIT_ASSERT(m_legality_check->legal(Nf3));
  
  ChessMove Bc4(Point(5, 7), Point(2, 4));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(Bc4));
  
  {
    ChessMove tmp(Point(5, 7), Point(2, 4));
    m_state->board().set(Point(4, 6), ChessPiece());
    CPPUNIT_ASSERT(m_legality_check->legal(tmp));
  }
  
  ChessMove e6(Point(4, 1), Point(4, 2));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(e6));
  
  {
    ChessMove tmp(Point(4, 1), Point(4, 2));
    m_state->switchTurn();
    CPPUNIT_ASSERT(m_legality_check->legal(tmp));
  }
}

void ChessLegalityTest::test_promotion() {
  m_state->board().set(Point(7, 1), ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
  ChessMove h8Q(Point(7, 1), Point(7, 0), ChessPiece::QUEEN);
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(h8Q));
  
  ChessMove hxg8R(Point(7, 1), Point(6, 0), ChessPiece::ROOK);
  CPPUNIT_ASSERT(m_legality_check->pseudolegal(hxg8R));
  CPPUNIT_ASSERT(m_legality_check->legal(hxg8R));
  CPPUNIT_ASSERT_EQUAL((int)ChessPiece::ROOK, hxg8R.promoteTo());
}

void ChessLegalityTest::test_en_passant() {
  m_state->move(ChessMove(Point(4, 6), Point(4, 4)));
  m_state->move(ChessMove(Point(7, 1), Point(7, 2)));
  m_state->move(ChessMove(Point(4, 4), Point(4, 3)));
  
  ChessMove d5(Point(3, 1), Point(3, 3));
  CPPUNIT_ASSERT(m_legality_check->legal(d5));
  CPPUNIT_ASSERT(d5.enPassantTrigger() == Point(3, 2));
  m_state->move(d5);
  
  ChessMove exd6(Point(4, 3), Point(3, 2));
  CPPUNIT_ASSERT(m_legality_check->legal(exd6));
  CPPUNIT_ASSERT(exd6.captureSquare() == Point(3, 3));
  
  m_state->move(ChessMove(Point(7, 6), Point(7, 5)));
  m_state->move(ChessMove(Point(7, 2), Point(7, 3)));
  
  ChessMove tmp(Point(4, 3), Point(3, 2));
  CPPUNIT_ASSERT(!m_legality_check->pseudolegal(tmp));
}

void ChessLegalityTest::test_castling() {
  ChessMove oo(Point(4, 7), Point(6, 7));
  
  m_state->board().set(Point(6, 7), ChessPiece());
  {
    ChessMove tmp(oo);
    CPPUNIT_ASSERT(!m_legality_check->pseudolegal(tmp));
  }
  
  m_state->board().set(Point(5, 7), ChessPiece());
  {
    ChessMove tmp(oo);
    CPPUNIT_ASSERT(m_legality_check->legal(tmp));
  }
  
  m_state->board().set(Point(3, 6), ChessPiece(ChessPiece::BLACK, ChessPiece::PAWN));
  {
    ChessMove tmp(oo);
    CPPUNIT_ASSERT(!m_legality_check->pseudolegal(tmp));
  }
  
  m_state->board().set(Point(3, 6), ChessPiece());
  m_state->board().set(Point(4, 6), ChessPiece(ChessPiece::BLACK, ChessPiece::PAWN));
  {
    ChessMove tmp(oo);
    CPPUNIT_ASSERT(!m_legality_check->pseudolegal(tmp));
  }
}

void ChessLegalityTest::test_attack1() {
  m_state->board().set(Point(5, 5), ChessPiece(ChessPiece::BLACK, ChessPiece::ROOK));
  CPPUNIT_ASSERT(m_legality_check->attacks(ChessPiece::WHITE, Point(5, 5)));  
}

void ChessLegalityTest::test_attack2() {
  m_state->board().set(Point(4, 4), ChessPiece(ChessPiece::BLACK, ChessPiece::QUEEN));
  CPPUNIT_ASSERT(!m_legality_check->attacks(ChessPiece::WHITE, Point(4, 4)));
}

void ChessLegalityTest::test_attack3() {
  m_state->board().set(Point(0, 5), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(1, 6), ChessPiece(ChessPiece::WHITE, ChessPiece::ROOK)); // b2: pawn -> rook
  CPPUNIT_ASSERT(m_legality_check->attacks(ChessPiece::WHITE, Point(0, 5)));
  
  m_state->board().set(Point(1, 7), ChessPiece()); // remove knight on b1
  CPPUNIT_ASSERT(!m_legality_check->attacks(ChessPiece::WHITE, Point(0, 5)));
  
  m_state->board().set(Point(0, 6), ChessPiece()); // remove pawn on a2
  CPPUNIT_ASSERT(m_legality_check->attacks(ChessPiece::WHITE, Point(0, 5)));
  
  m_state->board().set(Point(0, 7), ChessPiece()); // remove rook on a1
  CPPUNIT_ASSERT(!m_legality_check->attacks(ChessPiece::WHITE, Point(0, 5)));
}

void ChessLegalityTest::test_attack4() {
  CPPUNIT_ASSERT(!m_legality_check->attacks(ChessPiece::BLACK, Point(4, 7)));
}

