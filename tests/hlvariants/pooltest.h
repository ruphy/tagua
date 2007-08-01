#ifndef POOLTEST_H
#define POOLTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

namespace HLVariant {
  template <typename T> class Pool;
  template <typename T> class PoolCollection;
  namespace Chess {
    class Piece;
  }
}

typedef HLVariant::Chess::Piece ChessPiece;
typedef HLVariant::Pool<ChessPiece> ChessPool;
typedef HLVariant::PoolCollection<ChessPool> ChessPoolCollection;

class PoolTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(PoolTest);
  CPPUNIT_TEST(test_empty_pool);
  CPPUNIT_TEST(test_add);
  CPPUNIT_TEST(test_remove);
  CPPUNIT_TEST(test_remove_clear);
  CPPUNIT_TEST(test_empty_remove);
  CPPUNIT_TEST(test_pool_equality);
  CPPUNIT_TEST(test_collection_equality);
  CPPUNIT_TEST_SUITE_END();
private:
  ChessPoolCollection* m_pools;
public:
  void setUp();
  void tearDown();
  
  void test_empty_pool();
  void test_add();
  void test_remove();
  void test_remove_clear();
  void test_empty_remove();
  void test_pool_equality();
  void test_collection_equality();
};

#endif // POOLTEST_H

