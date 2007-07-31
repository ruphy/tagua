#include "chessserializationtest.h"
#include "common.h"

CPPUNIT_TEST_SUITE_REGISTRATION(ChessSerializationTest);

void ChessSerializationTest::setUp() {
  m_state = new ChessGameState;
  m_check = new ChessCheck(*m_state);
}

void ChessSerializationTest::tearDown() {
  delete m_check;
  delete m_state;
}

void ChessSerializationTest::test_pawn() {
  m_state->setup();
  
  ChessMove move(Point(4, 6), Point(4, 4));
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("e4"), san.serialize(move, *m_state));
    
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("e2e4"), simple.serialize(move, *m_state));
}

void ChessSerializationTest::test_check() {
  m_state->board().set(Point(5, 5), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(0, 0), ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  m_state->board().set(Point(3, 5), ChessPiece(ChessPiece::WHITE, ChessPiece::BISHOP));
  
  ChessMove move(Point(3, 5), Point(4, 4));
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("Be4+"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("d3e4"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("{bishop}e4+"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_check_capture() {
  m_state->board().set(Point(5, 5), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(0, 0), ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  m_state->board().set(Point(3, 5), ChessPiece(ChessPiece::WHITE, ChessPiece::BISHOP));
  m_state->board().set(Point(4, 4), ChessPiece(ChessPiece::BLACK, ChessPiece::ROOK));
  
  ChessMove move(Point(3, 5), Point(4, 4));
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("Bxe4+"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("d3e4"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("{bishop}xe4+"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_promotion() {
  m_state->board().set(Point(4, 7), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(0, 0), ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  m_state->board().set(Point(7, 1), ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
  
  ChessMove move(Point(7, 1), Point(7, 0), ChessPiece::ROOK);
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("h8=R"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("h7h8=R"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("h8={rook}"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_promotion_capture() {
  m_state->board().set(Point(4, 7), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(0, 0), ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  m_state->board().set(Point(7, 1), ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
  m_state->board().set(Point(6, 0), ChessPiece(ChessPiece::BLACK, ChessPiece::BISHOP));
  
  ChessMove move(Point(7, 1), Point(6, 0), ChessPiece::ROOK);
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("hxg8=R"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("h7g8=R"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("hxg8={rook}"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_promotion_check() {
  m_state->board().set(Point(4, 0), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(0, 0), ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  m_state->board().set(Point(7, 1), ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
  
  ChessMove move(Point(7, 1), Point(7, 0), ChessPiece::ROOK);
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("h8=R+"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("h7h8=R"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("h8={rook}+"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_promotion_capture_check() {
  m_state->board().set(Point(4, 0), ChessPiece(ChessPiece::BLACK, ChessPiece::KING));
  m_state->board().set(Point(0, 0), ChessPiece(ChessPiece::WHITE, ChessPiece::KING));
  m_state->board().set(Point(7, 1), ChessPiece(ChessPiece::WHITE, ChessPiece::PAWN));
  m_state->board().set(Point(6, 0), ChessPiece(ChessPiece::BLACK, ChessPiece::BISHOP));
  
  ChessMove move(Point(7, 1), Point(6, 0), ChessPiece::ROOK);
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("hxg8=R+"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("h7g8=R"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("hxg8={rook}+"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_castling_k() {
  m_state->setup();
  
  m_state->board().set(Point(5, 7), ChessPiece());
  m_state->board().set(Point(6, 7), ChessPiece());
  
  ChessMove move(Point(4, 7), Point(6, 7));
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("O-O"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("e1g1"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("O-O"), dec.serialize(move, *m_state));
}

void ChessSerializationTest::test_castling_q() {
  m_state->setup();
  
  m_state->board().set(Point(3, 7), ChessPiece());
  m_state->board().set(Point(2, 7), ChessPiece());
  m_state->board().set(Point(1, 7), ChessPiece());
  
  ChessMove move(Point(4, 7), Point(2, 7));
  CPPUNIT_ASSERT(m_check->legal(move));
  
  ChessSerializer san(ChessSerializer::COMPACT);
  CPPUNIT_ASSERT_EQUAL(QString("O-O-O"), san.serialize(move, *m_state));
  
  ChessSerializer simple(ChessSerializer::SIMPLE);
  CPPUNIT_ASSERT_EQUAL(QString("e1c1"), simple.serialize(move, *m_state));
  
  ChessSerializer dec(ChessSerializer::DECORATED);
  CPPUNIT_ASSERT_EQUAL(QString("O-O-O"), dec.serialize(move, *m_state));
}

