#include "chessmovetest.h"
#include "hlvariant/chess/move.h"
#include "hlvariant/chess/piece.h"

typedef HLVariant::Chess::Piece ChessPiece;

CPPUNIT_TEST_SUITE_REGISTRATION(ChessMoveTest);

void ChessMoveTest::setUp() { }

void ChessMoveTest::tearDown() { }

void ChessMoveTest::test_capture_square() {
  ChessMove m(Point(4, 3), Point(3, 2));
  CPPUNIT_ASSERT(m.captureSquare() == Point(3, 2));
  
  m.setType(ChessMove::EN_PASSANT_CAPTURE);
  CPPUNIT_ASSERT(m.captureSquare() == Point(3, 3));
}

void ChessMoveTest::test_en_passant_trigger() {
  ChessMove m(Point(4, 6), Point(4, 4));
  m.setType(ChessMove::EN_PASSANT_TRIGGER);
  CPPUNIT_ASSERT(m.enPassantTrigger() == Point(4, 5));
  
  ChessMove m2(Point(3, 1), Point(3, 3));
  m2.setType(ChessMove::EN_PASSANT_TRIGGER);
  CPPUNIT_ASSERT(m2.enPassantTrigger() == Point(3, 2));
}

void ChessMoveTest::test_promotion() {
  ChessMove m(Point(5, 1), Point(5, 0), ChessPiece::ROOK);
  
  CPPUNIT_ASSERT_EQUAL(-1, m.promoteTo());
  m.setType(ChessMove::PROMOTION);
  CPPUNIT_ASSERT_EQUAL((int)ChessPiece::ROOK, m.promoteTo());
}

