#include "weakset.h"
#include <boost/test/unit_test.hpp>
using boost::unit_test::test_suite;

template <typename T>
uint weak_set_size(weak_set<T>& w) {
  uint c = 0;
  for (typename weak_set<T>::iterator i = w.begin();
       i != w.end(); ++i) {
    ++c;
  }
  return c;
}

void test_insertion() {
  boost::shared_ptr<int> n(new int(8));
  boost::shared_ptr<int> m(new int(1));
  weak_set<int> s;
  s.insert(n);
  s.insert(m);
  
  BOOST_CHECK_EQUAL(2, weak_set_size(s));
}

void test_garbage_collection() {
  #ifdef WEAK_SET_DEBUG
    cout << "\n\ntesting garbage collection\n\n" << endl;
  #endif // WEAK_SET_DEBUG
  
  weak_set<int> s;
    
  boost::shared_ptr<int> u(new int(37));
  s.insert(u);
    
  {
    boost::shared_ptr<int> m(new int(1));
    s.insert(m);
  }
  
  boost::shared_ptr<int> n(new int(8));
  s.insert(n);
  
  BOOST_CHECK_EQUAL(2, weak_set_size(s));
}

void test_get() {
  weak_set<int> s;
  boost::shared_ptr<int> n(new int(42));
 
  s.insert(n);
  
  BOOST_CHECK_EQUAL(42, *s.begin());
}

void test_empty() {
  weak_set<int> s;
  
  {
    boost::shared_ptr<int> m(new int(37));
    s.insert(m);
  }
  
  BOOST_CHECK_EQUAL(0, weak_set_size(s));
}

test_suite*
init_unit_test_suite(int, char*[]) {
  test_suite* test= BOOST_TEST_SUITE( "Weak set test suite" );

  test->add(BOOST_TEST_CASE(&test_insertion), 0);
  test->add(BOOST_TEST_CASE(&test_garbage_collection), 0);
  test->add(BOOST_TEST_CASE(&test_get), 0);
  test->add(BOOST_TEST_CASE(&test_empty), 0);

  return test;
}

