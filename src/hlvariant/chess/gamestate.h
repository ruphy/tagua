#ifndef HLVARIANT__CHESS__GAMESTATE_H
#define HLVARIANT__CHESS__GAMESTATE_H

#include "../board.h"
#include "piece.h"
#include "move.h"
#include "nopool.h"
#include "export.h"

namespace HLVariant {
namespace Chess {

struct TAGUA_EXPORT CastlingData {
  bool wk : 1;
  bool wq : 1;
  bool bk : 1;
  bool bq : 1;
  
  CastlingData();
  CastlingData(bool, bool, bool, bool);
  bool operator==(const CastlingData& other) const;
};

template <typename _Board, typename _Move>
class GameState {
public:
  typedef _Board Board;
  typedef _Move Move;
  typedef typename Board::Piece Piece;
  typedef NoPool Pool;
protected:
  Board m_board;
  CastlingData m_castling;
  Point m_en_passant;
  typename Piece::Color m_turn;
public:
  GameState();
  GameState(typename Piece::Color, bool, bool, bool, bool, const Point&);
  virtual ~GameState() { }
  
  virtual Board& board();
  virtual const Board& board() const;
  
  virtual void setup();
  
  virtual bool operator==(const GameState<Board, Move>& other) const;
  
  virtual Point enPassant() const;
  virtual bool kingCastling(typename Piece::Color color) const;
  virtual bool queenCastling(typename Piece::Color color) const;
  
  virtual void move(const Move& m);
  virtual void basicMove(const Move& m);
  virtual void handleCastling(const Piece& piece, const Move& m);
  virtual void captureOn(const Point& p);
  
  virtual void setTurn(typename Piece::Color color);
  virtual typename Piece::Color previousTurn() const;
  virtual void switchTurn();
  virtual typename Piece::Color turn() const;
  
  virtual int startingRank(typename Piece::Color color) const;
  virtual int promotionRank(typename Piece::Color color) const;
  virtual Point kingStartingPosition(typename Piece::Color color) const;
  virtual Point direction(typename Piece::Color color) const;
};

// IMPLEMENTATION

template <typename Board, typename Move>
GameState<Board, Move>::GameState()
: m_board(Point(8, 8))
, m_en_passant(Point::invalid())
, m_turn(Piece::WHITE) { }

template <typename Board, typename Move>
GameState<Board, Move>::GameState(
  typename Piece::Color turn, 
  bool wkCastle, 
  bool wqCastle, 
  bool bkCastle, 
  bool bqCastle, 
  const Point& ep)
: m_board(Point(8, 8))
, m_castling(wkCastle, wqCastle, bkCastle, bqCastle)
, m_en_passant(ep)
, m_turn(turn) { }

template <typename Board, typename Move>
Board& GameState<Board, Move>::board() { return m_board; }

template <typename Board, typename Move>
const Board& GameState<Board, Move>::board() const { return m_board; }

template <typename Board, typename Move>
Point GameState<Board, Move>::enPassant() const {
  return m_en_passant;
}

template <typename Board, typename Move>
bool GameState<Board, Move>::kingCastling(typename Piece::Color color) const {
  if (color == Piece::WHITE) {
    return m_castling.wk;
  }
  else {
    return m_castling.bk;
  }
}

template <typename Board, typename Move>
bool GameState<Board, Move>::queenCastling(typename Piece::Color color) const {
  if (color == Piece::WHITE) {
    return m_castling.wq;
  }
  else {
    return m_castling.bq;
  }
}

template <typename Board, typename Move>
void GameState<Board, Move>::setup() {
  for (int c = 0; c < 2; c++) {
    typename Piece::Color color = static_cast<typename Piece::Color>(c);
    int rank = startingRank(color);
    for (int i = 0; i < m_board.size().x; i++) {
      m_board.set(Point(i, rank + direction(color).y), 
                  Piece(color, Piece::PAWN));
      m_board.set(Point(0, rank), Piece(color, Piece::ROOK));
      m_board.set(Point(1, rank), Piece(color, Piece::KNIGHT));
      m_board.set(Point(2, rank), Piece(color, Piece::BISHOP));
      m_board.set(Point(3, rank), Piece(color, Piece::QUEEN));
      m_board.set(Point(4, rank), Piece(color, Piece::KING));
      m_board.set(Point(5, rank), Piece(color, Piece::BISHOP));
      m_board.set(Point(6, rank), Piece(color, Piece::KNIGHT));
      m_board.set(Point(7, rank), Piece(color, Piece::ROOK));
    }
  }
}

template <typename Board, typename Move>
bool GameState<Board, Move>::operator==(const GameState<Board, Move>& other) const {
  return m_board == other.m_board &&
         m_castling == other.m_castling &&
         m_en_passant == other.m_en_passant;
}

template <typename Board, typename Move>
void GameState<Board, Move>::captureOn(const Point& p) {
  m_board.set(p, Piece());
}

template <typename Board, typename Move>
void GameState<Board, Move>::basicMove(const Move& m) {
  if (m.from() != m.to()) {
    m_board.set(m.to(), m_board.get(m.from()));
    m_board.set(m.from(), Piece());
  }
}

template <typename Board, typename Move>
void GameState<Board, Move>::handleCastling(const Piece& piece, const Move& m) {
  if (m.kingSideCastling()) {
    Point rookSquare = m.to() + Point(1,0);
    Point rookDestination = m.from() + Point(1,0);
    basicMove(Move(rookSquare, rookDestination));
  }

  else if (m.queenSideCastling()) {
    Point rookSquare = m.to() - Point(2,0);
    Point rookDestination = m.from() - Point(1,0);
    basicMove(Move(rookSquare, rookDestination));
  }
   
  typename Piece::Type type = piece.type();
  typename Piece::Color color = piece.color();
  
  if (type == Piece::KING) {
    if (color == Piece::WHITE) {
      m_castling.wk = false;
      m_castling.wq = false;
    }
    else {
      m_castling.bk = false;
      m_castling.bq = false;
    }
  }

  if (m.from() == Point(0,0) || m.to() == Point(0,0))
    m_castling.bq = false;
  else if (m.from() == Point(7,0) || m.to() == Point(7,0))
    m_castling.bk = false;
  else if (m.from() == Point(0,7) || m.to() == Point(0,7))
    m_castling.wq  = false;
  else if (m.from() == Point(7,7) || m.to() == Point(7,7))
    m_castling.wk = false;
}

template <typename Board, typename Move>
void GameState<Board, Move>::switchTurn() {
  m_turn = Piece::oppositeColor(m_turn);
}

template <typename Board, typename Move>
void GameState<Board, Move>::move(const Move& m) {
  Piece piece = m_board.get(m.from());
  if (piece == Piece()) return;

  captureOn(m.captureSquare());
  basicMove(m);

  m_en_passant = m.enPassantTrigger();
  
  typename Piece::Type promotion_type = 
    static_cast<typename Piece::Type>(m.promoteTo());
  if (promotion_type != Piece::INVALID_TYPE) {
    m_board.set(m.to(), Piece(piece.color(), promotion_type));
  }
  
  handleCastling(piece, m);
  switchTurn();
}

template <typename Board, typename Move>
typename Board::Piece::Color GameState<Board, Move>::turn() const {
  return m_turn;
}

template <typename Board, typename Move>
typename Board::Piece::Color GameState<Board, Move>::previousTurn() const {
  return Piece::oppositeColor(m_turn);
}

template <typename Board, typename Move>
void GameState<Board, Move>::setTurn(typename Piece::Color color) {
  m_turn = color;
}

template <typename Board, typename Move>
int GameState<Board, Move>::startingRank(typename Piece::Color color) const {
  return color == Piece::WHITE ? m_board.size().y - 1 : 0;
}

template <typename Board, typename Move>
int GameState<Board, Move>::promotionRank(typename Piece::Color color) const {
  return startingRank(Piece::oppositeColor(color));
}

template <typename Board, typename Move>
Point GameState<Board, Move>::kingStartingPosition(typename Piece::Color color) const {
  return Point(4, startingRank(color));
}

template <typename Board, typename Move>
Point GameState<Board, Move>::direction(typename Piece::Color color) const {
  return Point(0, color == Piece::WHITE ? -1 : 1);
}


} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__GAMESTATE_H

