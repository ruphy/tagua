#ifndef CHESSMOVETEST_H
#define CHESSMOVETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

// forward decl
namespace HLVariant {
  namespace Chess {
    class Move;
  }
}

typedef HLVariant::Chess::Move ChessMove;

class ChessMoveTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ChessMoveTest);
  CPPUNIT_TEST(test_capture_square);
  CPPUNIT_TEST(test_en_passant_trigger);
  CPPUNIT_TEST(test_promotion);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp();
  void tearDown();
  
  void test_capture_square();
  void test_en_passant_trigger();
  void test_promotion();
};

#endif // CHESSMOVETEST_H

