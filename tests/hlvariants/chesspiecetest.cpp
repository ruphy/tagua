#include "chesspiecetest.h"
#include "prototype/chess/piece.h"

typedef HLVariant::Chess::Piece ChessPiece;

CPPUNIT_TEST_SUITE_REGISTRATION(ChessPieceTest);

void ChessPieceTest::setUp() { }

void ChessPieceTest::tearDown() { }

void ChessPieceTest::test_basic() {
  ChessPiece p(ChessPiece::WHITE, ChessPiece::ROOK);
  CPPUNIT_ASSERT_EQUAL(ChessPiece::WHITE, p.color());
  CPPUNIT_ASSERT_EQUAL(ChessPiece::ROOK, p.type());
}

void ChessPieceTest::test_names() {
  ChessPiece p(ChessPiece::BLACK, ChessPiece::BISHOP);
  CPPUNIT_ASSERT(p.colorName() == "black");
  CPPUNIT_ASSERT(p.typeName() == "bishop");
  CPPUNIT_ASSERT(p.name() == "black_bishop");
}

void ChessPieceTest::test_compare() {
  ChessPiece a(ChessPiece::BLACK, ChessPiece::KING);
  ChessPiece b(ChessPiece::WHITE, ChessPiece::QUEEN);
  ChessPiece c(ChessPiece::BLACK, ChessPiece::KING);
  
  CPPUNIT_ASSERT(a != b);
  CPPUNIT_ASSERT(!(a == b));
  CPPUNIT_ASSERT(a == c);
  CPPUNIT_ASSERT(c == a);
  CPPUNIT_ASSERT(a == a);
}

