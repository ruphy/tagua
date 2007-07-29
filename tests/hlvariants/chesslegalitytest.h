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
  CPPUNIT_TEST(test_simple_move);
  CPPUNIT_TEST_SUITE_END();
private:
  ChessGameState* m_state;
  ChessLegalityCheck* m_legality_check;
public:
  void setUp();
  void tearDown();
  
  void test_simple_move();
};

#endif // CHESSGAMESTATETEST_H

