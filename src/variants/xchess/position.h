/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef POSITION_H
#define POSITION_H

#include <vector>
#include <map>
#include <iostream>
#include <memory>

#include <QString>
#include <QStringList>
#include <boost/shared_ptr.hpp>

#include "common.h"
#include "pathinfo.h"
#include "option.h"
#include "interactiontype.h"
#include "turntest.h"
#include "variants/xchess/generator.h"
#include "variants/xchess/piecetype.h"

class AlgebraicNotation;
class VerboseNotation;

#define POLYMORPHIC_NEW(type, argsdecl, args) \
  virtual type* _new argsdecl const { return new type args; }

template <typename M, typename P, typename B>
class Position;

template <typename M, typename P, typename B>
class Position {
public:
  typedef typename P::Type  Type;
  typedef typename P::Color Color;
protected:
  Color m_turn;

  B m_board;
  Point m_enPassantSquare;

  bool m_castleWhiteKing : 1;
  bool m_castleWhiteQueen : 1;
  bool m_castleBlackKing : 1;
  bool m_castleBlackQueen : 1;

public:
  typedef M Move;
  typedef P Piece;
  typedef B Board;

  Position(int = 8, int = 8);
  Position(const OptList& l, int = 8, int = 8);
  Position(Color turn,
          bool castleWhiteKing, bool castleWhiteQueen,
          bool castleBlackKing, bool castleBlackQueen,
          const Point& enPassant);
  Position(const Position<M, P, B>&);
  template <typename M1, typename P1, typename B1>
  Position(const Position<M1, P1, B1>&);

  Point size() const { return m_board.getSize(); }

  POLYMORPHIC_NEW(Position, (), ())
  POLYMORPHIC_NEW(Position, (const Position* other), (*other))
  virtual Position* clone() const { return new Position(*this); }
  virtual ~Position();

  virtual QString type() const { return "chess"; }

  bool operator==(const Position<M, P, B>&) const;
  bool operator!=(const Position<M, P, B>&) const;

  virtual void setup();

  void debugInfo() const;

  virtual const Point first() const { return m_board.first(); }
  virtual const Point last() const { return m_board.last(); }
  virtual Point next(const Point& p) const { return m_board.next(p); }
  inline bool valid(const Point& p) const { return m_board.valid(p); }

  PathInfo path(const Point&, const Point&) const;
  const Point& enPassantSquare() const { return m_enPassantSquare; }

  virtual P get(const Point& p) const {
    return valid(p) ? m_board[p] : P();
  }
  virtual P operator[](const Point& p) const { return get(p); }
  inline void set(const Point& p, const P& piece) {
    if(!valid(p))
      return;
    m_board[p] = piece;
  }
  inline void removePiece(const Point& p) { set(p, 0); }
  inline void basicMovePiece(const M&);

  inline InteractionType movable(const TurnTest& test, const Point& p) const {
    if (!valid(p) || !m_board[p] || !test(m_board[p].color()))
      return NoAction;
      
    return m_board[p].color() == m_turn ? Moving : Premoving;
  }
  inline InteractionType droppable(const TurnTest& test, int p) const {
    if (!test(m_turn))
      return NoAction;
      
    Color c = static_cast<Color>(p);
    return c == m_turn ? Moving : Premoving;
  }
  inline Color turn() const { return m_turn; }
  inline Color previousTurn() const { return Piece::oppositeColor(m_turn); }
  inline const B& board() const { return m_board; }
  inline bool castleKing(Color color) const { return color == WHITE? m_castleWhiteKing : m_castleBlackKing; }
  inline bool castleQueen(Color color) const { return color == WHITE? m_castleWhiteQueen : m_castleBlackQueen; }

  virtual bool attacks(Color, const Point& destination, Point& source) const;
  virtual bool attacks(Color, const Point& destination) const;
  virtual bool attacks(const Point& destination) const;

  Point findPiece(Color, typename P::Type) const;
  virtual void switchTurn();
  virtual void setTurn(Color turn);

  typename P::Type promotionType() const { return QUEEN; }

  virtual void move(const M&);
  virtual Position<M, P, B>* legallyMove(M&) const;
  void stepBack(const Position<M, P, B>* oldPosition, const M& move);
  virtual Color moveTurn(const M& move) const;
  virtual bool checkPromotionType(const M&) const;
  virtual bool pseudolegal(M&) const;
  virtual bool testMove(M&) const;
  virtual bool testPremove(M) const;

  virtual M getMove(const AlgebraicNotation& san, bool& ok) const;
//  virtual M getMove(const QString& san, bool& ok) const;
  static M getVerboseMove(Color, const VerboseNotation& m);

  //FIXME: Those are really very chess dependent, make them virtual at least
  static Point kingStartingPosition(Color color) { return color == WHITE? Point(4,7) : Point(4,0); }

  inline M kingCastleMove() const { return m_turn == WHITE?
    M(Point(4, 7), Point(6, 7)) : M(Point(4,0), Point(6,0)); }
  inline M queenCastleMove() const { return m_turn == WHITE?
    M(Point(4, 7), Point(2, 7)) : M(Point(4,0), Point(2,0)); }

  enum State {
    InPlay,
    WhiteWins,
    BlackWins,
    Draw
  };
  State state() const;
  virtual boost::shared_ptr<AbstractGenerator<M> > createLegalGenerator() const;
  virtual bool stalled() const;
//   virtual bool pseudoStalled() const;
  bool check() const;
  bool check(Color) const;

  virtual void basicDropPiece(P* piece, const Point& to);
  virtual void executeCaptureOn(const Point& point);

  boost::shared_ptr<P> moveHint(const M&) const;

  void dump() const;
  QString fen(int, int) const;
  void fromFEN(const QString& fen, bool&);
  QStringList borderCoords() const;
};

std::ostream& operator<<(std::ostream& os, const class GenericPosition&);


#include <iostream>
#include "algebraicnotation.h"

template <typename M, typename P, typename B>
Position<M, P, B>::Position(int width, int height)
: m_turn(WHITE)
, m_board(width, height)
, m_enPassantSquare(Point::invalid())
, m_castleWhiteKing(true)
, m_castleWhiteQueen(true)
, m_castleBlackKing(true)
, m_castleBlackQueen(true) { }

template <typename M, typename P, typename B>
Position<M, P, B>::Position(const OptList&, int width, int height)
: m_turn(WHITE)
, m_board(width, height)
, m_enPassantSquare(Point::invalid())
, m_castleWhiteKing(true)
, m_castleWhiteQueen(true)
, m_castleBlackKing(true)
, m_castleBlackQueen(true) { }

template <typename M, typename P, typename B>
Position<M, P, B>::Position(Color turn, bool castleWhiteKing,
                                        bool castleWhiteQueen,
                                        bool castleBlackKing,
                                        bool castleBlackQueen,
                                        const Point& enPassant)
: m_turn(turn)
, m_board(8, 8)
, m_enPassantSquare(enPassant)
, m_castleWhiteKing(castleWhiteKing)
, m_castleWhiteQueen(castleWhiteQueen)
, m_castleBlackKing(castleBlackKing)
, m_castleBlackQueen(castleBlackQueen) { }

template <typename M, typename P, typename B>
Position<M, P, B>::Position(const Position<M, P, B>& other)
: m_turn(other.m_turn)
, m_board(other.m_board)
, m_enPassantSquare(other.m_enPassantSquare)
, m_castleWhiteKing(other.m_castleWhiteKing)
, m_castleWhiteQueen(other.m_castleWhiteQueen)
, m_castleBlackKing(other.m_castleBlackKing)
, m_castleBlackQueen(other.m_castleBlackQueen) { }

template <typename M, typename P, typename B>
template <typename M1, typename P1, typename B1>
Position<M, P, B>::Position(const Position<M1, P1, B1>& other)
: m_turn(other.turn())
, m_board(other.board())
, m_enPassantSquare(other.enPassantSquare())
, m_castleWhiteKing(other.castleKing(WHITE))
, m_castleWhiteQueen(other.castleQueen(WHITE))
, m_castleBlackKing(other.castleKing(BLACK))
, m_castleBlackQueen(other.castleQueen(BLACK)) { }


template <typename M, typename P, typename B>
Position<M, P, B>::~Position() {

}

template <typename M, typename P, typename B>
QStringList Position<M, P, B>::borderCoords() const
{
  QStringList retv;
  Point p = m_board.getSize();
  for(int i=0; i<p.x; i++)
    retv << QChar('a'+i);
  for(int i=1; i<=p.y; i++)
    retv += QString::number(i);
  return retv + retv;
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::operator==(const Position<M, P, B>& other) const {
#if 0
  std::cout << "turn: " << m_turn << " " << other.m_turn << std::endl;
  std::cout << "ep: " << m_enPassantSquare << " " << other.m_enPassantSquare << std::endl;
  std::cout << "wk: " << m_castleWhiteKing << " " << other.m_castleWhiteKing << std::endl;
  std::cout << "wq: " << m_castleWhiteQueen << " " << other.m_castleWhiteQueen << std::endl;
  std::cout << "bk: " << m_castleBlackKing << " " << other.m_castleBlackKing << std::endl;
  std::cout << "bq: " << m_castleBlackQueen << " " << other.m_castleBlackQueen << std::endl;
  if (m_turn != other.m_turn) {
    std::cout << "turns differ" << std::endl;
    return false;
  }
  if (!(m_board == other.m_board)) {
    std::cout << "boards differ" << std::endl;
    return false;
  }
  if (m_castleWhiteKing != other.m_castleWhiteKing) {
    std::cout << "wk differ" << std::endl;
    return false;
  }
  if (m_castleWhiteQueen != other.m_castleWhiteQueen) {
    std::cout << "wq differ" << std::endl;
    return false;
  }
  if (m_castleBlackKing != other.m_castleBlackKing) {
    std::cout << "bk differ" << std::endl;
    return false;
  }
  if (m_castleBlackQueen != other.m_castleBlackQueen) {
    std::cout << "bq differ" << std::endl;
    return false;
  }
  std::cout << "equal" << std::endl;
  return true;
#else
  return m_turn == other.m_turn &&
         m_board == other.m_board &&
         m_enPassantSquare == other.m_enPassantSquare &&
         m_castleWhiteKing == other.m_castleWhiteKing &&
         m_castleWhiteQueen == other.m_castleWhiteQueen &&
         m_castleBlackKing == other.m_castleBlackKing &&
         m_castleBlackQueen == other.m_castleBlackQueen;
#endif
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::operator!=(const Position& other) const {
  return !(*this == other);
}

template <typename M, typename P, typename B>
void Position<M, P, B>::debugInfo() const {
#if 0
  std::cout << "board = " << &m_board << std::endl;
#endif
}

#define SET_PIECE(i,j, color, type) m_board[Point(i,j)] = P(color, type)
template <typename M, typename P, typename B>
void Position<M, P, B>::setup() {
  for (int i = 0; i < 8; i++) {
    SET_PIECE(i, 1, BLACK, PAWN);
    SET_PIECE(i, 6, WHITE, PAWN);
  }

  SET_PIECE(0,0, BLACK, ROOK);
  SET_PIECE(1,0, BLACK, KNIGHT);
  SET_PIECE(2,0, BLACK, BISHOP);
  SET_PIECE(3,0, BLACK, QUEEN);
  SET_PIECE(4,0, BLACK, KING);
  SET_PIECE(5,0, BLACK, BISHOP);
  SET_PIECE(6,0, BLACK, KNIGHT);
  SET_PIECE(7,0, BLACK, ROOK);

  SET_PIECE(0,7, WHITE, ROOK);
  SET_PIECE(1,7, WHITE, KNIGHT);
  SET_PIECE(2,7, WHITE, BISHOP);
  SET_PIECE(3,7, WHITE, QUEEN);
  SET_PIECE(4,7, WHITE, KING);
  SET_PIECE(5,7, WHITE, BISHOP);
  SET_PIECE(6,7, WHITE, KNIGHT);
  SET_PIECE(7,7, WHITE, ROOK);

  m_turn = WHITE;
  m_enPassantSquare = Point::invalid();
  m_castleWhiteKing = true;
  m_castleWhiteQueen = true;
  m_castleBlackKing = true;
  m_castleBlackQueen = true;
}
#undef SET_PIECE

template <typename M, typename P, typename B>
PathInfo Position<M, P, B>::path(const Point& from, const Point& to) const {
  return m_board.path(from, to);
}
/*
template <typename M, typename P, typename B>
void Position<M, P, B>::movePiece(const M& move) {
  executeCaptureOn(move.to);
  basicMovePiece(move);
  switchTurn();
}*/

template <typename M, typename P, typename B>
void Position<M, P, B>::executeCaptureOn(const Point& point) {
  m_board[point] = Piece();
}

template <typename M, typename P, typename B>
void Position<M, P, B>::basicMovePiece(const M& move) {
  P p = m_board[move.from];
  Q_ASSERT(p);

  m_board[move.to] = p;
  m_board[move.from] = Piece();
}

template <typename M, typename P, typename B>
void Position<M, P, B>::basicDropPiece(P* piece, const Point& to) {
  Q_ASSERT(piece);
  m_board[to] = *piece;
  switchTurn();
}

template <typename M, typename P, typename B>
boost::shared_ptr<P> Position<M, P, B>::moveHint(const M& move) const {
  if (move.type() == M::Promotion)
    return boost::shared_ptr<P>(new Piece(m_turn, move.promotionType));
  else
    return boost::shared_ptr<P>();
}


template <typename M, typename P, typename B>
bool Position<M, P, B>::checkPromotionType(const M& m) const {
  return (m.promotionType == KNIGHT ||
     m.promotionType == BISHOP ||
     m.promotionType == ROOK  ||
     m.promotionType == QUEEN);
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::pseudolegal(M& move) const {
  if (!move.valid()) return false;

  if (!valid(move.from)) return false;
  if (!valid(move.to)) return false;
  P piece = m_board[move.from];
  Q_ASSERT(piece);
  Color thisTurn = piece.color();
  Color otherTurn = P::oppositeColor(thisTurn);
  if (piece && (turn() == thisTurn)) {
    move.setType(piece.canMove(*this, move.from, move.to));

    if (move.type() == M::Invalid) return false;

    if (move.type() == M::Promotion && !checkPromotionType(move))
      return false;

    if (move.type() == M::KingSideCastling) {
      if (attacks(otherTurn, move.from) ||
          attacks(otherTurn, move.from + Point(1,0))) return false;
    }
    else if (move.type() == M::QueenSideCastling) {
      if (attacks(otherTurn, move.from) ||
          attacks(otherTurn, move.from + Point(-1,0))) return false;
    }

    return true;
  }
  else return false;
}

template <typename M, typename P, typename B>
typename P::Color Position<M, P, B>::moveTurn(const M& move) const {
  P piece = m_board[move.from];
  Q_ASSERT(piece);
  return piece.color();
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::testMove(M& move) const {

  if (move.status == M::Untested) {
    if (pseudolegal(move)) {
      Color turn = moveTurn(move);

      // check king safety
      std::auto_ptr<Position> tempPosition(clone());
      tempPosition->move(move);
      Point kingPos = tempPosition->findPiece(turn, KING);

      if (kingPos == Point::invalid())
        move.status = M::Illegal;
      else if (tempPosition->attacks(P::oppositeColor(turn), kingPos))
          move.status = M::Illegal;
      else
          move.status = M::Legal;
    }
    else
      move.status = M::Illegal;
  }

  return move.status == M::Legal;
}

template <typename M, typename P, typename B>
M Position<M, P, B>::getMove(const AlgebraicNotation& san, bool& ok) const {
  M candidate = M::invalid();

  if (san.invalid()) {
    ok = false;
    return candidate;
  }

  if (san.castling != AlgebraicNotation::NoCastling) {
    Point from = kingStartingPosition(turn());
    Point to = from + (san.castling == AlgebraicNotation::KingSide? Point(2,0) : Point(-2,0));
    P king = m_board[from];
    if (!(king && king.type() == KING)) {
      ok = false;
      return candidate;
    }
    else {
      ok = true;
      return M(from, to);
    }
  }

  if (san.from.valid()) {
    candidate = M(san.from, san.to);
  }
  else {
    for (Point i = first(); i <= last(); i = next(i)) {
      P p = m_board[i];
      M mv(i, san.to, static_cast<typename P::Type>(san.promotion));
      if (i.resembles(san.from) && p && p.type() == san.type && p.color() == turn()
          && testMove(mv)) {

        if (candidate.valid()) {
          ok = false;
          return candidate; // return the first matching move
        }
        else candidate = mv;
      }
    }
  }

  ok = candidate.valid();
  return candidate;
}
/*
template <typename M, typename P, typename B>
M Position<M, P, B>::getMove(const QString& san, bool& ok) const {
  return getMove(AlgebraicNotation(san), ok);
}*/

template <typename M, typename P, typename B>
M Position<M, P, B>::getVerboseMove(Color turn, const VerboseNotation& m) {
  Point from, to;
  if (m.castling == AlgebraicNotation::NoCastling) {
    from = m.from;
    to = m.to;
  }
  else {
    from = kingStartingPosition(turn);
    to = from + (m.castling == AlgebraicNotation::KingSide ? Point(2,0) : Point(-2, 0));
  }

  M res(from, to);
  res.promotionType = static_cast<typename P::Type>(m.promotion);

  return res;
}

template <typename M, typename P, typename B>
Position<M, P, B>* Position<M, P, B>::legallyMove(M& mv) const {
  if (testMove(mv)) {
    Position* res = clone();
    res->move(mv);
    return res;
  }
  else
    return 0;
}

template <typename M, typename P, typename B>
void Position<M, P, B>::fromFEN(const QString& fen, bool& ok) {
  ok = false;

  QStringList data = fen.split(' ', QString::SkipEmptyParts);
  const int boardIndex = 0;
  const int turnIndex = 1;
  const int castlingIndex = 2;
  const int enPassantIndex = 3;

  // read turn
  QChar turn = data[turnIndex][0].toLower();
  if (turn == 'w')
    setTurn(WHITE);
  else if (turn == 'b')
    setTurn(BLACK);

  // read castling
  for (int i = 0; i < data[castlingIndex].length(); i++) {
    QChar symbol = data[castlingIndex][i];
    if (symbol == 'k')
      m_castleBlackKing = true;
    else if (symbol == 'q')
      m_castleBlackQueen = true;
    else if (symbol == 'K')
      m_castleWhiteKing = true;
    else if (symbol == 'Q')
      m_castleWhiteQueen = true;
  }

  // read en-passant
  m_enPassantSquare = Point(data[enPassantIndex], size().y);

  // read board
  Point cursor(0, 0);
  for (int index = 0; index < data[boardIndex].length(); index++) {
    QChar symbol = data[boardIndex][index];
    if (symbol == '/') {
      cursor = Point(0, cursor.y + 1);
    }
    else if (symbol.isDigit()) {
      int k = symbol.toAscii() - '0';
      cursor.x += k;
    }
    else {
      if (!valid(cursor)) return;
      typename P::Type type = P::getType(symbol);
      if (type == INVALID_TYPE) return;
      Color color = symbol.isUpper() ? WHITE : BLACK;
      set(cursor, P(color, type));
      cursor.x++;
    }
  }

  ok = true;
}

// adapted from Maurizio Monge's rattatechess
template <typename M, typename P, typename B>
QString Position<M, P, B>::fen(int halfmove, int fullmove) const {
  QString str;

  // add board information
  int cn = 0;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      Point p = Point(j, i);
      Piece pc = m_board[p];
      if (!pc)
        cn++;
      else {
        if (cn > 0) {
          str += QString::number(cn);
          cn = 0;
        }
        QString symbol = P::typeSymbol(pc.type());
        if (pc.color() == BLACK)
          symbol = symbol.toLower();
        str += symbol;
      }
    }
    if (cn > 0) {
      str += QString::number(cn);
      cn = 0;
    }
    if (i != 7)
      str += "/";
  }

  // add turn information
  str += turn() == WHITE ? " w " : " b ";

  // add castling information
  QString castleString;
  if(m_castleWhiteKing)
      castleString += "K";
  if(m_castleWhiteQueen)
      castleString += "Q";
  if(m_castleBlackKing)
      castleString += "k";
  if(m_castleBlackQueen)
      castleString += "k";
  if (castleString == "") castleString = "-";
  str += castleString + " ";

  // add en passant information
  if (m_enPassantSquare == Point::invalid())
    str += "-";
  else
    str += m_enPassantSquare.toString(size().y);
  str += " ";

  // add move count information
  str += QString::number(halfmove) + " " + QString::number(fullmove);

  return str;
}


template <typename M, typename P, typename B>
void Position<M, P, B>::dump() const {
  for (Point i = first(); i <= last(); i = next(i)) {
    if (i.x == 0)
      std::cout << "+---+---+---+---+---+---+---+---+" << std::endl;

    Piece piece = m_board[i];
    QString symbol;
    if (piece) {
      symbol = Piece::typeSymbol(piece.type());
      if (piece.color() == BLACK) symbol = symbol.toLower();
    }
    else
      symbol = ((i.x + i.y) % 2 == 0) ? " " : ".";

    std::cout << "| " << symbol << " ";
    if (i.x == 7)
      std::cout << "|" << std::endl;
  }
  std::cout << "+---+---+---+---+---+---+---+---+" << std::endl;
}

template <typename M, typename P, typename B>
void Position<M, P, B>::move(const M& move) {
  P piece = m_board[move.from];
  Q_ASSERT(piece);

  Color color = piece.color();
  typename P::Type type = piece.type();

  executeCaptureOn(move.to);
  basicMovePiece(move);

  if (move.type() == M::EnPassantTrigger)
    m_enPassantSquare = (move.to + move.from) / 2;
  else {
    m_enPassantSquare = Point::invalid();

    if (move.type() == M::EnPassantCapture) {
      Point phantom(move.to.x, move.from.y);
      m_board[phantom] = Piece();
    }

    else if (move.type() == M::Promotion) {
      typename P::Type type = move.promotionType;
      m_board[move.to] = P(piece.color(), type);
    }

    else if (move.type() == M::KingSideCastling) {
      Point rookSquare = move.to + Point(1,0);
      Point rookDestination = move.from + Point(1,0);

      P rook = m_board[rookSquare];

      Q_ASSERT(rook);
      Q_ASSERT(rook.type() ==  ROOK);
      Q_UNUSED(rook);

      basicMovePiece(M(rookSquare, rookDestination));
    }

    else if (move.type() == M::QueenSideCastling) {
      Point rookSquare = move.to - Point(2,0);
      Point rookDestination = move.from - Point(1,0);

      P rook = m_board[rookSquare];

      Q_ASSERT(rook);
      Q_ASSERT(rook.type() == ROOK);
      Q_UNUSED(rook);

      basicMovePiece(M(rookSquare, rookDestination));
    }
  }

  if (type == KING) {
    if (color == WHITE) {
      m_castleWhiteKing = false;
      m_castleWhiteQueen = false;
    }
    else {
      m_castleBlackKing = false;
      m_castleBlackQueen = false;
    }
  }

  if (move.from == Point(0,0) || move.to == Point(0,0))
    m_castleBlackQueen = false;
  else if (move.from == Point(7,0) || move.to == Point(7,0))
    m_castleBlackKing = false;
  else if (move.from == Point(0,7) || move.to == Point(0,7))
    m_castleWhiteQueen = false;
  else if (move.from == Point(7,7) || move.to == Point(7,7))
    m_castleWhiteKing = false;

  switchTurn();
}

template <typename M, typename P, typename B>
void Position<M, P, B>::switchTurn() {
  m_turn = P::oppositeColor(turn());
}

template <typename M, typename P, typename B>
void Position<M, P, B>::setTurn(Color turn) {
  m_turn = turn;
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::testPremove(M premove) const {
  for (Point p = first(); p <= last(); p = next(p)) {
    P piece = m_board[p];
    if (!piece || piece.color() != turn()) continue;
    for (Point q = first(); q <= last(); q = next(q)) {
      M move(p,q);
      if (testMove(move)) {
        Position tempPosition(*this);
        tempPosition.move(move);

        if (tempPosition.testMove(premove)) return true;
      }
    }
  }

  return false;
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::attacks(Color color, const Point& destination) const {
  Point temp;
  return attacks(color, destination, temp);
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::attacks(Color color, const Point& destination, Point& source) const {
  for (Point p = first(); p <= last(); p = next(p)) {
    P piece = m_board[p];
    if (piece && piece.color() == color && piece.canMove(*this, p, destination)) {
      source = p;
      return true;
    }
  }
  return false;
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::attacks(const Point& destination) const {
  return attacks(turn(), destination);
}

template <typename M, typename P, typename B>
Point Position<M, P, B>::findPiece(Color color, typename P::Type type) const {
  for (int i = 0; i < 8; i++)
  for (int j = 0; j < 8; j++) {
    Point p = Point(i,j);
    P piece = m_board[p];
    if (piece && piece.color() == color && piece.type() == type)
      return p;
  }
  return Point::invalid();
}

template <typename M, typename P, typename B>
boost::shared_ptr<AbstractGenerator<M> >
Position<M, P, B>::createLegalGenerator() const {
  return boost::shared_ptr<AbstractGenerator<M> >(
    new MoveGenerator<Position, LegalMove<Position> >(*this));
}

// template <typename M, typename P, typename B>
// boost::shared_ptr<AbstractGenerator<M> > createPseudoLegalGenerator() const {
//   return boost::shared_ptr<AbstractGenerator<M> >(
//     new MoveGenerator<Position, LegalMove<Position> >(*this));
// }

template <typename M, typename P, typename B>
bool Position<M, P, B>::stalled() const {
  boost::shared_ptr<AbstractGenerator<M> > generator =
    createLegalGenerator();
  std::vector<M> mv = generator->generate();
  return generator->generate().size() == 0;
}
/*
template <typename M, typename P, typename B>
bool Position<M, P, B>::pseudoStalled() const {
  MoveGenerator<Position, PseudolegalMove<Position> > generator(*this);
  return generator.generate().size() == 0;
}*/

template <typename M, typename P, typename B>
bool Position<M, P, B>::check(Color turn) const {
  Point kingPosition = findPiece(turn, KING);
  if (kingPosition == Point::invalid()) {
    // a missing king is considered in check
    return true;
  }
  else
    return attacks(P::oppositeColor(turn), kingPosition);
}

template <typename M, typename P, typename B>
bool Position<M, P, B>::check() const {
  return check(turn());
}

template <typename M, typename P, typename B>
typename Position<M, P, B>::State Position<M, P, B>::state() const {
  if (!stalled()) return InPlay;

  if (check())
    return turn() == WHITE? BlackWins : WhiteWins;
  else
    return Draw;
}


//END Implementation








#endif // POSITION_H
