#include <boost/test/unit_test.hpp>
#include "global.h"
using boost::unit_test::test_suite;


void test_null() {
  BOOST_CHECK(!settings["dummy"]);
  BOOST_CHECK(!settings["fourtytwo"]);
}

void test_create_and_remove() {
  settings["dummy"] = QColor(3, 4, 5);
  BOOST_CHECK(settings["dummy"]);
  BOOST_CHECK(QColor(3, 4, 5) == settings["dummy"].value<QColor>());
  settings["dummy"].remove();
  BOOST_CHECK(!settings["dummy"]);
}

void test_map() {
  SettingMap<QString> map = settings.newMap<QString>("duck", "name");
  
  {
    Settings s = map.insert("paperino");
    s["age"] = 34;
  }
  {
    Settings s = map.insert("gastone");
    s["age"] = 29;
  }
  {
    Settings s = map.insert("qui");
    s["age"] = 12;
  }
  {
    Settings s = map.insert("quo");
    s["age"] = 12;
  }
  {
    Settings s = map.insert("qua");
    s["age"] = 12;
  }  
  
  BOOST_CHECK(map.size() == 5);
  Settings s = map.get("gastone");
  BOOST_CHECK(s["age"].value<int>() == 29);
   
  map.clear();
  
  BOOST_CHECK(!settings["duck"]);
}

void test_array() {
  SettingArray array = settings.newArray("duck");
  {
    Settings s = array.append();
    s["name"] = "nonna papera";
  }
  {
    Settings s = array.append();
    s["name"] = "ciccio";
  }
  
  BOOST_CHECK(array.size() == 2);
  Settings s = array.get(1);
  BOOST_CHECK(s["name"].value<QString>() == "ciccio");
  
  array.clear();
  
  BOOST_CHECK(!settings["duck"]);
}

test_suite*
init_unit_test_suite(int, char*[]) {
  test_suite* test= BOOST_TEST_SUITE("XML Settings test suite");

  test->add(BOOST_TEST_CASE(&test_null), 0);
  test->add(BOOST_TEST_CASE(&test_create_and_remove), 0);
  test->add(BOOST_TEST_CASE(&test_map), 0);
  test->add(BOOST_TEST_CASE(&test_array), 0);
  return test;
}
