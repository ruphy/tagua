#include "chessgamestatetest.h"
#include "hlvariant/chess/gamestate.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ChessGameStateTest);

void ChessGameStateTest::setUp() {
  m_state = new ChessGameState;
  m_state->setup();
}

void ChessGameStateTest::tearDown() {
  delete m_state;
}

void ChessGameStateTest::test_setup() {
  for (int i = 0; i < 8; i++) {
    CPPUNIT_ASSERT_EQUAL(ChessPiece::BLACK,
      m_state->board().get(Point(i, 0)).color());
    CPPUNIT_ASSERT_EQUAL(ChessPiece::BLACK,
      m_state->board().get(Point(i, 1)).color());
    CPPUNIT_ASSERT_EQUAL(ChessPiece::WHITE,
      m_state->board().get(Point(i, 6)).color());
    CPPUNIT_ASSERT_EQUAL(ChessPiece::WHITE,
      m_state->board().get(Point(i, 7)).color());
    CPPUNIT_ASSERT(m_state->board().get(Point(i, 4)) == ChessPiece());
  }
  
  CPPUNIT_ASSERT_EQUAL(ChessPiece::ROOK,
    m_state->board().get(Point(0, 0)).type());
  CPPUNIT_ASSERT_EQUAL(ChessPiece::KING,
    m_state->board().get(Point(4, 7)).type());
  CPPUNIT_ASSERT_EQUAL(ChessPiece::PAWN,
    m_state->board().get(Point(6, 6)).type());
}

void ChessGameStateTest::test_simple_move() {
  m_state->move(ChessMove(Point(4, 6), Point(4, 5))); // e4
  CPPUNIT_ASSERT(m_state->board().get(Point(4, 6)) == ChessPiece());
  CPPUNIT_ASSERT(m_state->board().get(Point(4, 5)) == 
    ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
}

void ChessGameStateTest::test_capture() {
  m_state->move(ChessMove(Point(4, 6), Point(4, 4))); // e4
  m_state->move(ChessMove(Point(3, 1), Point(3, 3))); // d5
  m_state->move(ChessMove(Point(4, 4), Point(3, 3))); // exd5
  
  CPPUNIT_ASSERT(m_state->board().get(Point(3, 3)) == 
    ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
}

void ChessGameStateTest::test_en_passant() {
  m_state->move(ChessMove(Point(4, 6), Point(4, 4))); // e4
  m_state->move(ChessMove(Point(7, 1), Point(7, 2))); // h6
  m_state->move(ChessMove(Point(4, 4), Point(4, 3))); // e5
  
  ChessMove d5(Point(3, 1), Point(3, 3));
  d5.setType(ChessMove::EN_PASSANT_TRIGGER);
  m_state->move(d5);
  
  CPPUNIT_ASSERT(m_state->enPassant() == Point(3, 2));
  
  ChessMove exd6(Point(4, 3), Point(3, 2));
  exd6.setType(ChessMove::EN_PASSANT_CAPTURE);
  m_state->move(exd6);
  
  CPPUNIT_ASSERT(m_state->board().get(Point(3, 3)) == ChessPiece());
}

void ChessGameStateTest::test_kingside_castling() {
  ChessMove oo(Point(4, 7), Point(6, 7));
  oo.setType(ChessMove::KING_SIDE_CASTLING);
  m_state->move(oo);
  
  CPPUNIT_ASSERT(m_state->board().get(Point(6, 7)) == 
    ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  CPPUNIT_ASSERT(m_state->board().get(Point(5, 7)) ==
    ChessPiece(ChessPiece::WHITE, ChessPiece::ROOK));
  CPPUNIT_ASSERT(m_state->board().get(Point(7, 7)) == ChessPiece());
}

void ChessGameStateTest::test_queenside_castling() {
  ChessMove oo(Point(4, 7), Point(2, 7));
  oo.setType(ChessMove::QUEEN_SIDE_CASTLING);
  m_state->move(oo);
  
  CPPUNIT_ASSERT(m_state->board().get(Point(2, 7)) == 
    ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  CPPUNIT_ASSERT(m_state->board().get(Point(3, 7)) ==
    ChessPiece(ChessPiece::WHITE, ChessPiece::ROOK));
  CPPUNIT_ASSERT(m_state->board().get(Point(0, 7)) == ChessPiece());
}

void ChessGameStateTest::test_promotion() {
  m_state->board().set(Point(7, 1), ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
  ChessMove h8B(Point(7, 1), Point(7, 0), ChessPiece::BISHOP);
  h8B.setType(ChessMove::PROMOTION);
  m_state->move(h8B);
  
  CPPUNIT_ASSERT(m_state->board().get(Point(7, 0)) ==
    ChessPiece(ChessPiece::WHITE, ChessPiece::BISHOP));
}


