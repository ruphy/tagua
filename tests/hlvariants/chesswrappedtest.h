#ifndef CHESSWRAPPEDTEST_H
#define CHESSWRAPPEDTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#include "fwd.h"

class ChessWrappedTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ChessWrappedTest);
  CPPUNIT_TEST(test_setup);
  CPPUNIT_TEST(test_move);
  CPPUNIT_TEST(test_movable);
  CPPUNIT_TEST_SUITE_END();
private:
  PositionPtr m_pos;
public:
  void setUp();
  void tearDown();
  
  void test_setup();
  void test_move();
  void test_movable();
};

#endif // CHESSWRAPPEDTEST_H
