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
    class Move;
    class Piece;
  }
}

typedef HLVariant::Chess::Move ChessMove;
typedef HLVariant::Chess::Piece ChessPiece;
typedef HLVariant::Board<ChessPiece> Chessboard;
typedef HLVariant::Chess::GameState<Chessboard, ChessMove> ChessGameState;

class ChessGameStateTest : public CppUnit::TestFixture {
  CPPUNIT_TEST_SUITE(ChessGameStateTest);
  CPPUNIT_TEST(test_setup);
  CPPUNIT_TEST(test_simple_move);
  CPPUNIT_TEST(test_capture);
  CPPUNIT_TEST(test_en_passant);
  CPPUNIT_TEST(test_kingside_castling);
  CPPUNIT_TEST(test_queenside_castling);
  CPPUNIT_TEST(test_promotion);
  CPPUNIT_TEST_SUITE_END();
private:
  ChessGameState* m_state;
public:
  void setUp();
  void tearDown();
  
  void test_setup();
  void test_simple_move();
  void test_capture();
  void test_en_passant();
  void test_kingside_castling();
  void test_queenside_castling();
  void test_promotion();
};

#endif // CHESSGAMESTATETEST_H

