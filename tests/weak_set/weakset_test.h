#ifndef WEAKSETTEST_H
#define WEAKSETTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

class WeaksetTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(WeaksetTest);
  CPPUNIT_TEST(test_insertion);
  CPPUNIT_TEST(test_garbage_collection);
  CPPUNIT_TEST(test_get);
  CPPUNIT_TEST(test_empty);
  CPPUNIT_TEST_SUITE_END();
public:
  void setUp() { }
  void tearDown() { }
  
  void test_insertion();
  void test_garbage_collection();
  void test_get();
  void test_empty();
};

#endif // WEAKSETTEST_H
