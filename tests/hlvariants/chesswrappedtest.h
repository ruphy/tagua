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
  CPPUNIT_TEST(test_san1);
  CPPUNIT_TEST(test_get_move1);
  CPPUNIT_TEST(test_fools_mate);
  CPPUNIT_TEST(test_check);
  CPPUNIT_TEST_SUITE_END();
private:
  PositionPtr m_pos;
public:
  void setUp();
  void tearDown();
  
  void test_setup();
  void test_move();
  void test_movable();
  void test_san1();
  void test_get_move1();
  void test_fools_mate();
  void test_check();
};

#endif // CHESSWRAPPEDTEST_H
