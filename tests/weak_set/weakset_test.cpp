#include "weakset_test.h"
#include "weakset.h"

CPPUNIT_TEST_SUITE_REGISTRATION(WeaksetTest);

template <typename T>
uint weak_set_size(weak_set<T>& w) {
  uint c = 0;
  for (typename weak_set<T>::iterator i = w.begin();
       i != w.end(); ++i) {
    ++c;
  }
  return c;
}

void WeaksetTest::test_insertion() {
  boost::shared_ptr<int> n(new int(8));
  boost::shared_ptr<int> m(new int(1));
  weak_set<int> s;
  s.insert(n);
  s.insert(m);
  
  CPPUNIT_ASSERT_EQUAL(2U, weak_set_size(s));
}

void WeaksetTest::test_garbage_collection() {
  weak_set<int> s;
    
  boost::shared_ptr<int> u(new int(37));
  s.insert(u);
    
  {
    boost::shared_ptr<int> m(new int(1));
    s.insert(m);
  }
  
  boost::shared_ptr<int> n(new int(8));
  s.insert(n);
  
  CPPUNIT_ASSERT_EQUAL(2U, weak_set_size(s));
}

void WeaksetTest::test_get() {
  weak_set<int> s;
  boost::shared_ptr<int> n(new int(42));
 
  s.insert(n);
  
  CPPUNIT_ASSERT_EQUAL(42, *s.begin());
}

void WeaksetTest::test_empty() {
  weak_set<int> s;
  
  {
    boost::shared_ptr<int> m(new int(37));
    s.insert(m);
  }
  
  CPPUNIT_ASSERT_EQUAL(0U, weak_set_size(s));
}

