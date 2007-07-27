#include "settingstest.h"
#include "mastersettings.h"

CPPUNIT_TEST_SUITE_REGISTRATION(SettingsTest);

MasterSettings& SettingsTest::test_settings() { 
  return *m_instance; 
}

void SettingsTest::setUp() {
  system("rm tmp.xml");
  m_instance = new MasterSettings("tmp.xml", MasterSettings::PathLookup);
}

void SettingsTest::tearDown() {
  delete m_instance;
}

void SettingsTest::test_null() {
  CPPUNIT_ASSERT(!test_settings()["dummy"]);
  CPPUNIT_ASSERT(!test_settings()["fourtytwo"]);
}

void SettingsTest::test_create_and_remove() {
  test_settings()["dummy"] = QColor(3, 4, 5);
  CPPUNIT_ASSERT(test_settings()["dummy"]);
  CPPUNIT_ASSERT(QColor(3, 4, 5) == test_settings()["dummy"].value<QColor>());
  test_settings()["dummy"].remove();
  CPPUNIT_ASSERT(!test_settings()["dummy"]);
}

void SettingsTest::test_map() {
  SettingMap<QString> map = test_settings().newMap<QString>("duck", "name");
  
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
  
  CPPUNIT_ASSERT(map.size() == 5);
  Settings s = map.get("gastone");
  CPPUNIT_ASSERT(s["age"].value<int>() == 29);
   
  map.clear();
  
  CPPUNIT_ASSERT(!test_settings()["duck"]);
}

void SettingsTest::test_array() {
  SettingArray array = test_settings().newArray("duck");
  {
    Settings s = array.append();
    s["name"] = "nonna papera";
  }
  {
    Settings s = array.append();
    s["name"] = "ciccio";
  }
  
  CPPUNIT_ASSERT(array.size() == 2);
  Settings s = array.get(1);
  CPPUNIT_ASSERT(s["name"].value<QString>() == "ciccio");
  
  array.clear();
  
  CPPUNIT_ASSERT(!test_settings()["duck"]);
}


