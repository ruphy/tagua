#ifndef HLVARIANT__CHESS__SERIALIZER_H
#define HLVARIANT__CHESS__SERIALIZER_H

#include "legalitycheck.h"

namespace HLVariant {
namespace Chess {

template <typename GameData>
class Serializer {
public:
  /**
    * Possible string representations for moves.
    */
  enum {
    SIMPLE = 0,          /// The most direct way of representing a move.
    COMPACT = 1,         /// Compact notation. This corresponds to SAN notation for games that support it.
    DECORATED = 2        /// Symbolic figurine notation. Figurine names are enclosed in braces.
  } MoveRepresentation;
private:
  typedef typename GameData::MoveGenerator MoveGenerator;
  typedef typename GameData::LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Move Move;
  typedef typename GameState::Board::Piece Piece;
  
  int m_rep;

protected:
  virtual QString suffix(const Move& move, const GameState& ref);
  
  virtual QString san(const Move& move, const GameState& ref);
public:
  /** 
    * Create a serializer to a given string representation for moves.
    * \param rep A move representation.
    * \sa MoveRepresentation.
    */
  Serializer(int rep);
  
  virtual ~Serializer();
  
  /**
    * Serialize a move given a reference game state.
    * We assume the move has already been tested against @a ref.
    * Calling this function on an untested or illegal move is safe but its return
    * value is undefined.
    * Do not try to call this function on an invalid move.
    * \param ref The position in which this move shall be executed.
    * \return A string representation for this move.
    */
  virtual QString serialize(const Move& move, const GameState& ref);
  
  /**
    * Convert a string representation of a move back to a move object.
    * \param ref The position in which this move shall be executed.
    * \return A move corresponding to the given string representation.
    */
  virtual Move deserialize(const QString& str, const GameState& ref);
};


// IMPLEMENTATION

template <typename GameData>
Serializer<GameData>::Serializer(int rep)
: m_rep(rep) { }

template <typename GameData>
Serializer<GameData>::~Serializer() { }

template <typename GameData>
QString Serializer<GameData>::san(const Move& move, const GameState& ref) {
  if (!move.valid())
    return "";
    
  QString res;
      
  Piece piece = ref.board().get(move.from());
  Piece captured = ref.board().get(move.captureSquare());

  if (piece == Piece())
    return ""; // no piece in the initial square

  Q_ASSERT(piece);

  if (move.kingSideCastling()) {
    res = "O-O";
  }
  else if (move.queenSideCastling()) {
    res = "O-O-O";
  }
  else {
    res = piece.typeName()[0].toUpper();

    SAN tmp;
    tmp.from = move.from();
    tmp.to = move.to();
    tmp.type = piece.type();
    tmp.castling = SAN::NoCastling;
    minimalNotation(tmp, ref);

    res += tmp.from.toString(ref.size().y);
    if (captured)
      res += "x";
    res += tmp.to.toString(m_ref.size().y);
  }

  if (move.promoteTo() != -1)
    res += "=" + Piece::typeName(move.promoteTo())[0].toUpper();

  res += checkSuffix();

  return res;
}

template <typename GameData>
QString Serializer<GameData>::serialize(const Move& move, const GameState& ref) {
  switch (m_ref) {
  case SIMPLE:
  case COMPACT:
    return san(move, ref);
  case DECORATED:
    {
      QString res = san(move, ref);
      res.replace('K', "{king}");
      res.replace('Q', "{queen}");
      res.replace('R', "{rook}");
      res.replace('N', "{knight}");
      res.replace('B', "{bishop}");
      res.replace('P', "{pawn}");
      return res;
    }
  default:
    return "";
  }
}

template <typename GameData>
QString Serializer<GameData>::suffix(const Move& move, const GameState& ref) {
  GameState tmp(ref);
  tmp.move(move);
  
  MoveGenerator generator(tmp);
  if (generator.check()) {
    if (generator.stalled())
      return "#";
    else
      return "+";
  }
  else {
    return "";
  }
}

template <typename GameData>
typename Serializer<GameData>::Move 
Serializer<GameData>::deserialize(const QString& str, const GameState& ref) {
  switch (m_rep) {
  case SIMPLE:
    return Move(); // BROKEN
  case COMPACT:
    {
      SAN tmp(str);
      Move candidate;
    
      if (san.invalid())
        return candidate;
    
      if (san.castling != SAN::NoCastling) {
        Point from = ref.kingStartingPosition(ref.turn());
        Point to = from + (san.castling == SAN::KingSide? Point(2,0) : Point(-2,0));
        Piece king = ref.board().get(from);
        if (king.type() != Piece::KING))
          return candidate;
        else
          return Move(from, to);
      }
    
      if (san.from.valid()) {
        candidate = Move(san.from, san.to, static_cast<typename Piece::Type>(san.promotion));
      }
      else {
        for (int i = 0; i < ref.board().size().x; i++) {
          for (int j = 0; j < ref.board().size().y; j++) {
            Point p(i, j);
            Piece piece = ref.board().get(p);
            
            Move mv(p, san.to, static_cast<typename Piece::Type>(san.promotion));
            if (p.resembles(san.from) && 
                piece.type() == san.type && 
                piece.color() == ref.turn()) {
      
              LegalityCheck check(ref);
              if (check.legal(mv))  {
                if (candidate.valid()) {
                  // ambiguous!
                  return Move();
                }
                else {
                  // ok, we have found a candidate move
                  candidate = mv;
                }
              }
            }
          }
        }
      }

      return candidate;
    }
  case DECORATED:
    // no need to parse decorated moves
    return Move();
  }
}

} // namespace Chess
} // namespace HLVariant

#endif // HLVARIANT__CHESS__SERIALIZER_H

