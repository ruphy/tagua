#include "boardtest.h"
#include "point.h"
#include "hlvariant/board.h"

CPPUNIT_TEST_SUITE_REGISTRATION(BoardTest);

void BoardTest::setUp() {
  m_board = new HLVariant::Board<int>(Point(8, 8));
}

void BoardTest::tearDown() {
  delete m_board;
}

void BoardTest::test_size() {
  CPPUNIT_ASSERT_EQUAL(8, m_board->size().x);
  CPPUNIT_ASSERT_EQUAL(8, m_board->size().y);
}

void BoardTest::test_get_null() {
  int x = m_board->get(Point(3, 2));
  CPPUNIT_ASSERT_EQUAL(int(), x);
}

void BoardTest::test_set_get() {
  m_board->set(Point(3, 2), 37);
  CPPUNIT_ASSERT_EQUAL(37, m_board->get(Point(3, 2)));
}

void BoardTest::test_valid() {
  CPPUNIT_ASSERT(!m_board->valid(Point(-1, 3)));
  CPPUNIT_ASSERT(!m_board->valid(Point(7, -1)));
  CPPUNIT_ASSERT(!m_board->valid(Point(8, 0)));
  CPPUNIT_ASSERT(!m_board->valid(Point(8, 7)));
  
  CPPUNIT_ASSERT(m_board->valid(Point(1, 3)));
  CPPUNIT_ASSERT(m_board->valid(Point(4, 7)));
  CPPUNIT_ASSERT(m_board->valid(Point(0, 0)));
  CPPUNIT_ASSERT(m_board->valid(Point(7, 7)));
}

void BoardTest::test_set_invalid() {
  m_board->set(Point(8, 2), 42);
  CPPUNIT_ASSERT_EQUAL(int(), m_board->get(Point(8, 2)));
}

void BoardTest::test_compare() {
  m_board->set(Point(3, 4), 3);
  m_board->set(Point(5, 1), 7);
  
  HLVariant::Board<int> other(m_board->size());
  other.set(Point(3, 4), 3);
  
  CPPUNIT_ASSERT(other != (*m_board));
  CPPUNIT_ASSERT(!(other == (*m_board)));
  
  other.set(Point(5, 1), 7);
  
  CPPUNIT_ASSERT(other == (*m_board));
}

void BoardTest::test_clone() {
  m_board->set(Point(3, 4), 23);
  
  HLVariant::Board<int> other(*m_board);
  
  CPPUNIT_ASSERT_EQUAL(23, other.get(Point(3, 4)));
  
  m_board->set(Point(3, 5), 21);
  
  CPPUNIT_ASSERT_EQUAL(int(), other.get(Point(3, 5)));
}


void BoardTest::test_pathinfo_h() {
  PathInfo path = m_board->path(Point(3, 4), Point(6, 4));
  CPPUNIT_ASSERT(path.direction() == PathInfo::Horizontal);
  CPPUNIT_ASSERT(path.parallel());
}

void BoardTest::test_pathinfo_v() {
  PathInfo path = m_board->path(Point(7, 2), Point(7, 4));
  CPPUNIT_ASSERT(path.direction() == PathInfo::Vertical);
  CPPUNIT_ASSERT(path.parallel());
}

void BoardTest::test_pathinfo_d() {
  PathInfo path = m_board->path(Point(3, 3), Point(5, 5));
  CPPUNIT_ASSERT(path.diagonal());

  path = m_board->path(Point(2, 7), Point(5, 4));
  CPPUNIT_ASSERT(path.diagonal());
}

void BoardTest::test_pathinfo_invalid() {
  PathInfo path = m_board->path(Point(-1, 3), Point(7, 3));
  CPPUNIT_ASSERT(!path.valid());
  
  path = m_board->path(Point(3, 3), Point(5, 4));
  CPPUNIT_ASSERT(!path.valid());
}

void BoardTest::test_pathinfo_obstacles() {
  PathInfo path = m_board->path(Point(1, 1), Point(4, 4));
  CPPUNIT_ASSERT(path.clear());
  
  m_board->set(Point(2, 2), 37);
  path = m_board->path(Point(1, 1), Point(4, 4));
  CPPUNIT_ASSERT(!path.clear());
  CPPUNIT_ASSERT_EQUAL(1, path.numObstacles());
  
  m_board->set(Point(6, 2), 42);
  path = m_board->path(Point(0, 2), Point(5, 2));
  CPPUNIT_ASSERT_EQUAL(1, path.numObstacles());
  
  path = m_board->path(Point(0, 2), Point(6, 2));
  CPPUNIT_ASSERT_EQUAL(1, path.numObstacles());
  
  path = m_board->path(Point(0, 2), Point(7, 2));
  CPPUNIT_ASSERT_EQUAL(2, path.numObstacles());
  
  path = m_board->path(Point(2, 2), Point(6, 2));
  CPPUNIT_ASSERT(path.clear());
}

void BoardTest::test_find() {
  m_board->set(Point(3, 4), 32);
  m_board->set(Point(5, 6), 22);
  m_board->set(Point(7, 7), 22);
  
  CPPUNIT_ASSERT(m_board->find(17) == Point::invalid());
  CPPUNIT_ASSERT(m_board->find(22) == Point(5, 6));
  CPPUNIT_ASSERT(m_board->find(32) == Point(3, 4));
}





