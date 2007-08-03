#ifndef SHOGIDESERIALIZATIONTEST_H
#define SHOGIDESERIALIZATIONTEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

#include "hlvariant/shogi/variant.h"
#include "hlvariant/variantdata.h"

using namespace HLVariant;

typedef VariantData<Shogi::Variant>::GameState ShogiGameState;
typedef VariantData<Shogi::Variant>::Serializer ShogiSerializer;
typedef VariantData<Shogi::Variant>::Piece ShogiPiece;
typedef VariantData<Shogi::Variant>::Move ShogiMove;

class ShogiDeserializationTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ShogiDeserializationTest);
  CPPUNIT_TEST(regression_P_drop_2c);
  CPPUNIT_TEST_SUITE_END();
private:
  ShogiGameState* m_state;
public:
  void setUp();
  void tearDown();
  
  void regression_P_drop_2c();
};

#endif // SHOGIDESERIALIZATIONTEST_H


