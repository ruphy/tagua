#ifndef CHESSSERIALIZATIONTEST_H
#define CHESSSERIALIZATIONTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#include "prototype/variantdata.h"
#include "prototype/chess/variant.h"

typedef HLVariant::Chess::Variant Chess;
typedef VariantData<Chess>::LegalityCheck ChessCheck;
typedef VariantData<Chess>::GameState ChessGameState;
typedef VariantData<Chess>::Serializer ChessSerializer;
typedef VariantData<Chess>::Piece ChessPiece;
typedef VariantData<Chess>::Move ChessMove;

class ChessSerializationTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ChessSerializationTest);
  CPPUNIT_TEST(test_pawn);
  CPPUNIT_TEST(test_check);
  CPPUNIT_TEST(test_check_capture);
  CPPUNIT_TEST(test_promotion);
  CPPUNIT_TEST(test_promotion_capture);
  CPPUNIT_TEST(test_promotion_check);
  CPPUNIT_TEST(test_promotion_capture_check);
  CPPUNIT_TEST(test_castling_k);
  CPPUNIT_TEST(test_castling_q);
  CPPUNIT_TEST_SUITE_END();
private:
  ChessGameState* m_state;
  ChessCheck* m_check;
public:
  void setUp();
  void tearDown();

  void test_pawn();
  void test_check();
  void test_check_capture();
  void test_promotion();
  void test_promotion_capture();
  void test_promotion_check();
  void test_promotion_capture_check();
  void test_castling_k();
  void test_castling_q();
};

#endif // CHESSSERIALIZATIONTEST_H
