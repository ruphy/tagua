#include "pooltest.h"

#include "hlvariant/pool.h"
#include "hlvariant/poolcollection.h"
#include "hlvariant/chess/piece.h"

CPPUNIT_TEST_SUITE_REGISTRATION(PoolTest);

void PoolTest::setUp() {
  m_pools = new ChessPoolCollection;
}

void PoolTest::tearDown() {
  delete m_pools;
}

void PoolTest::test_empty_pool() {
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::WHITE).empty());
  
  CPPUNIT_ASSERT_EQUAL(0, m_pools->pool(ChessPiece::WHITE).count(ChessPiece::KING));
}

void PoolTest::test_add() {
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::KNIGHT);
  CPPUNIT_ASSERT_EQUAL(1, m_pools->pool(ChessPiece::WHITE).count(ChessPiece::KNIGHT));
  
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::KNIGHT);
  CPPUNIT_ASSERT_EQUAL(2, m_pools->pool(ChessPiece::WHITE).count(ChessPiece::KNIGHT));
}

void PoolTest::test_remove() {
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  
  m_pools->pool(ChessPiece::WHITE).remove(ChessPiece::ROOK);
  CPPUNIT_ASSERT_EQUAL(1, m_pools->pool(ChessPiece::WHITE).count(ChessPiece::ROOK));
}

void PoolTest::test_remove_clear() {
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  
  m_pools->pool(ChessPiece::WHITE).remove(ChessPiece::ROOK);
  CPPUNIT_ASSERT_EQUAL(0, m_pools->pool(ChessPiece::WHITE).count(ChessPiece::ROOK));
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::WHITE).empty());
}

void PoolTest::test_empty_remove() {
  m_pools->pool(ChessPiece::WHITE).remove(ChessPiece::BISHOP);
  CPPUNIT_ASSERT_EQUAL(0, m_pools->pool(ChessPiece::WHITE).count(ChessPiece::ROOK));
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::WHITE).empty());
}

void PoolTest::test_pool_equality() {
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::WHITE) != m_pools->pool(ChessPiece::BLACK));
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::WHITE) == m_pools->pool(ChessPiece::WHITE));
  
  ChessPoolCollection other;
  other.pool(ChessPiece::BLACK).add(ChessPiece::KING);
  
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::BLACK) != other.pool(ChessPiece::BLACK));
  
  other.pool(ChessPiece::BLACK).remove(ChessPiece::KING);
  
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::BLACK) == other.pool(ChessPiece::BLACK));
  
  other.pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  
  CPPUNIT_ASSERT(m_pools->pool(ChessPiece::WHITE) == other.pool(ChessPiece::WHITE));
}

void PoolTest::test_collection_equality() {
  CPPUNIT_ASSERT((*m_pools) == (*m_pools));
  
  ChessPoolCollection other;
  other.pool(ChessPiece::BLACK).add(ChessPiece::KING);
  
  CPPUNIT_ASSERT((*m_pools) != other);
  
  other.pool(ChessPiece::BLACK).remove(ChessPiece::KING);
  
  CPPUNIT_ASSERT((*m_pools) == other);
  
  other.pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  m_pools->pool(ChessPiece::WHITE).add(ChessPiece::ROOK);
  
  CPPUNIT_ASSERT((*m_pools) == other);
}

