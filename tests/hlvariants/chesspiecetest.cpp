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

