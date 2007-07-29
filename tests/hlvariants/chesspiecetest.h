#ifndef CHESSPIECETEST_H
#define CHESSPIECETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

namespace HLVariant { namespace Chess { class Piece; } }

class ChessPieceTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ChessPieceTest);
  CPPUNIT_TEST(test_basic);
  CPPUNIT_TEST(test_names);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp();
  void tearDown();
  
  void test_basic();
  void test_names();
};

#endif // CHESSPIECETEST_H

