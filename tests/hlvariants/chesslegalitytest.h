#ifndef CHESSGAMESTATETEST_H
#define CHESSGAMESTATETEST_H

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>

// forward decl
namespace HLVariant { 
  template <typename Piece> class Board;
  
  namespace Chess { 
    template <typename Board, typename Move> class GameState; 
    template <typename State> class LegalityCheck;
    class Move;
    class Piece;
  }
}

typedef HLVariant::Chess::Move ChessMove;
typedef HLVariant::Chess::Piece ChessPiece;
typedef HLVariant::Board<ChessPiece> Chessboard;
typedef HLVariant::Chess::GameState<Chessboard, ChessMove> ChessGameState;
typedef HLVariant::Chess::LegalityCheck<ChessGameState> ChessLegalityCheck;

class ChessLegalityTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ChessLegalityTest);
  CPPUNIT_TEST(test_movements);
  CPPUNIT_TEST(test_pseudolegal);
  CPPUNIT_TEST(test_simple_move);
  CPPUNIT_TEST(test_promotion);
  CPPUNIT_TEST(test_en_passant);
  CPPUNIT_TEST(test_castling);
  CPPUNIT_TEST(test_attack1);
  CPPUNIT_TEST(test_attack2);
  CPPUNIT_TEST(test_attack3);
  CPPUNIT_TEST(test_attack4);
  CPPUNIT_TEST_SUITE_END();
private:
  ChessGameState* m_state;
  ChessLegalityCheck* m_legality_check;
public:
  void setUp();
  void tearDown();
  
  void test_movements();
  void test_pseudolegal();
  void test_simple_move();
  void test_promotion();
  void test_en_passant();
  void test_castling();
  void test_attack1();
  void test_attack2();
  void test_attack3();
  void test_attack4();
};

#endif // CHESSGAMESTATETEST_H


