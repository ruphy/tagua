#ifndef SETTINGSTEST_H
#define SETTINGSTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

class MasterSettings;

class SettingsTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(SettingsTest);
  CPPUNIT_TEST(test_null);
  CPPUNIT_TEST(test_create_and_remove);
  CPPUNIT_TEST(test_map);
  CPPUNIT_TEST(test_array);
  CPPUNIT_TEST_SUITE_END();
private:
  MasterSettings* m_instance;
  MasterSettings& test_settings();
public:
  void setUp();
  void tearDown();
  
  void test_null();
  void test_create_and_remove();
  void test_map();
  void test_array();
};

#endif // SETTINGSTEST_H
