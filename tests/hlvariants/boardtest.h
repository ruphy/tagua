#ifndef BOARDTEST_H
#define BOARDTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

namespace HLVariant {
  template <typename Piece> class Board;
}

class BoardTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(BoardTest);
  CPPUNIT_TEST(test_size);
  CPPUNIT_TEST(test_get_null);
  CPPUNIT_TEST(test_set_get);
  CPPUNIT_TEST(test_valid);
  CPPUNIT_TEST(test_set_invalid);
  CPPUNIT_TEST(test_compare);
  CPPUNIT_TEST(test_clone);
  CPPUNIT_TEST(test_find);
  
  CPPUNIT_TEST(test_pathinfo_h);
  CPPUNIT_TEST(test_pathinfo_v);
  CPPUNIT_TEST(test_pathinfo_d);
  CPPUNIT_TEST(test_pathinfo_invalid);
  CPPUNIT_TEST(test_pathinfo_obstacles);
  CPPUNIT_TEST_SUITE_END();
private:
  HLVariant::Board<int>* m_board;
public:
  void setUp();
  void tearDown();
  
  // basic stuff
  void test_size();
  void test_get_null();
  void test_set_get();
  void test_valid();
  void test_set_invalid();
  void test_compare();
  void test_clone();
  void test_find();
  
  // path info
  void test_pathinfo_h();
  void test_pathinfo_v();
  void test_pathinfo_d();
  void test_pathinfo_invalid();
  void test_pathinfo_obstacles();
};

#endif // BOARDTEST_H

