/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HLVARIANT__SHOGI__SERIALIZER_H
#define HLVARIANT__SHOGI__SERIALIZER_H

#include <QString>
#include <QRegExp>

namespace HLVariant {
namespace Shogi {

template <typename _LegalityCheck>
class Serializer {
  static QRegExp pattern;
public:
  typedef _LegalityCheck LegalityCheck;
  typedef typename LegalityCheck::GameState GameState;
  typedef typename GameState::Board Board;
  typedef typename Board::Piece Piece;
  typedef typename GameState::Move Move;
protected:
  QString m_rep;
  
  virtual bool isAmbiguous(const Move& move, const GameState& ref) const;
  virtual QString square(const Point& p, const Point& size) const;
  virtual QString symbol(const Piece& piece) const;
  virtual typename Piece::Type getType(const QChar& letter) const;
public:
  Serializer(const QString& rep);
  virtual ~Serializer();
  
  QString serialize(const Move&, const GameState& ref);
  Move deserialize(const QString& str, const GameState& ref);

  Move parse(const QString&, int& offset, int ysize, const GameState& ref);
  Move parse(const QString&, int ysize, const GameState& ref);
};

// IMPLEMENTATION

template <typename LegalityCheck>
Serializer<LegalityCheck>::Serializer(const QString& rep)
: m_rep(rep) { }


template <typename LegalityCheck>
Serializer<LegalityCheck>::~Serializer() { }

template <typename LegalityCheck>
bool Serializer<LegalityCheck>::isAmbiguous(const Move& move, const GameState& ref) const {
  Piece piece = move.drop();
  if (piece == Piece())
    piece = ref.board().get(move.from());
    
  bool ambiguous = false;
  if (move.drop() == Piece()) {
    for (int i = 0; i < ref.board().size().x; i++) {
      for (int j = 0; j < ref.board().size().x; j++) {
        Point p(i, j);
        if (p == move.from() || ref.board().get(p) != piece)
          continue;
        Move mv(p, move.to());
        LegalityCheck check(ref);
        if (check.legal(mv)) {
          ambiguous = true;
          break;
        }
      }
    }
  }
  
  return ambiguous;
}

template <typename LegalityCheck>
QString Serializer<LegalityCheck>::square(const Point& p, const Point& size) const {
  QString res = QString::number(size.x - p.x);
  if (m_rep == "decorated") {
    res += "{num_" + QString::number(p.y + 1) + "}";
  }
  else {
    res += QString(p.y + 'a');
  }
  
  return res;
}


template <typename LegalityCheck>
QString Serializer<LegalityCheck>::serialize(const Move& move, const GameState& ref) {
  Piece piece = move.drop();
  if (piece == Piece())
    piece = ref.board().get(move.from());

  QString res;
  
  if (m_rep == "simple") {
    if (move.drop() != Piece()) {
      res += symbol(piece);
      res += '*';
    } else
      res += square(move.from(), ref.board().size());
    res += square(move.to(), ref.board().size());
    if (move.promoteTo() != -1)
      res += "+";
    return res;
  }
  else {
    bool ambiguous = isAmbiguous(move, ref);
    
    QString res;
    if (piece.promoted())
      res += "+";
  
    res += symbol(piece);
    
    if (ambiguous) {
      res += square(move.from(), ref.board().size());
    }
    
    if (move.drop() != Piece())
      res += "*";
    else if (ref.board().get(move.to()) != Piece())
      res += "x";
    else
      res += "-";
      
    res += square(move.to(), ref.board().size());
    
    // if it is a promotion
    if (move.promoteTo() != -1)
      res += "+";
    // if it is a refused promotion
    else if (ref.canPromote(piece) && 
            move.drop() == Piece() &&
            ref.promotionZone(ref.turn(), move.to())) {
      res += "=";
    }
    
    return res;
  }
}

template <typename LegalityCheck>
QString Serializer<LegalityCheck>::symbol(const Piece& piece) const {
  if (m_rep == "decorated") {
    QString res = "{";
    if (piece.promoted())
      res += "p_";
    res += piece.typeName();
    if (piece.type() == Piece::KING)
      res += piece.color() == Piece::BLACK ? '1' : '2';
    return res + "}";
  }
  else {
    if (piece.type() == Piece::KNIGHT)
      return "N";
    else
      return piece.typeName()[0].toUpper();
  }
}

template <typename LegalityCheck>
typename Serializer<LegalityCheck>::Piece::Type 
Serializer<LegalityCheck>::getType(const QChar& letter) const {
  switch(letter.toLower().toAscii()) {
  case 'p':
    return Piece::PAWN;
  case 'r':
    return Piece::ROOK;
  case 'b':
    return Piece::BISHOP;
  case 'l':
    return Piece::LANCE;
  case 'n':
    return Piece::KNIGHT;
  case 's':
    return Piece::SILVER;
  case 'g':
    return Piece::GOLD;
  case 'k':
    return Piece::KING;
  default:
    return Piece::INVALID_TYPE;
  }
}

template <typename LegalityCheck>
QRegExp Serializer<LegalityCheck>::
//          1  2         3       4   5      6          7
  pattern("^(([+])?([PRBLNSGK]))?((\\d*)([a-wyzA-Z])?)([-x*])?"
//              8           9
	  "(\\d+[a-zA-Z])?([+=])?[\?!]*");

template <typename LegalityCheck>
typename Serializer<LegalityCheck>::Move
Serializer<LegalityCheck>::parse(const QString& str, int& offset,
				      int ysize, const GameState& ref) {
  std::cerr << "Looking at " << qPrintable(str) << std::endl;
  if (pattern.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    Point from;
    typename Serializer<LegalityCheck>::Piece::Type type;
    bool promoted;
    int promotion;
    Move candidate;

    type = getType(pattern.cap(3)[0]);
    promoted = pattern.cap(2) == "+";
    Point to(ref.board().size().x - pattern.cap(8)[0].digitValue(),
	     pattern.cap(8)[1].toAscii() - 'a');
    promotion = (pattern.cap(9) == "+") ? 1 : -1;

    if (pattern.cap(7) == "*")			  // is a drop ?
      return Move(Piece(ref.turn(), type), to);

    from = Point((pattern.cap(5).length() == 0) ? -1 :
		 ref.board().size().x - pattern.cap(5)[0].digitValue(),
		 (pattern.cap(6).length() == 0) ? -1 :
		 pattern.cap(6)[0].toAscii() - 'a');

    if (from.valid()) {				  // explicit from ?
      candidate = Move(from, to, static_cast<typename Piece::Type>(promotion));
      std::cerr << "from " << from.x << "," << from.y
		<< " already valid, to" << to.x << "," << to.y
		<< std::endl;
    }
    else { // resolve implicit from
      std::cerr << "Pattern is " << qPrintable(pattern.cap(8))
		<< ", type is " << type << (promoted ? "+" : "")
		<< ", from is " << from.x << "," << from.y
		<< ", target is " << to.x << "," << to.y
		<< std::endl;

      for (int i = 0; i < ref.board().size().x; i++) {
	for (int j = 0; j < ref.board().size().y; j++) {
	  Point p(i, j);
	  Piece piece = ref.board().get(p);
	  std::cerr << "looking at " << i << "," << j
		    << ", piece is " << piece.type()
		    << (piece.promoted() ? "+" : "")
		    << std::endl;

	  Move mv(p, to, static_cast<typename Piece::Type>(promotion));
	  if (p.resembles(from) &&
	      piece.type() == type &&
	      piece.promoted() == promoted &&
	      piece.color() == ref.turn()) {

	    LegalityCheck check(ref);
	    if (check.legal(mv))  {
	      if (candidate.valid()) {
		// ambiguous!
		std::cerr << "ambiguous !" << std::endl;
		return Move();
	      }
	      else {
		// ok, we have found a candidate move
		std::cerr << "candidate moved from "
			  << mv.from().x << "," << mv.from().y
			  << std::endl;
		candidate = mv;
	      }
	    }
	    else std::cerr << "piece at " << i << "," << j
			   << " cannot move to " << to.toString(ref.board().size().y)
			   << " aka " << to.x << "," << to.y
			   << std::endl;
	  }
	  else std::cerr << "skipping" << std::endl;
	}
      }
    }

    if (!candidate.valid())
      std::cerr << "error - piece not found" << std::endl;
    else
      std::cerr << "move is " << serialize(candidate, ref)
		<< " from " << candidate.from().x << "," << candidate.from().y
		<< std::endl;

    offset += pattern.matchedLength();
    return candidate;
  }
  else {
    std::cout << "error!!!! " << qPrintable(str.mid(offset)) << std::endl;
    return Move(Point::invalid(),Point::invalid());
  }
}

template <typename LegalityCheck>
typename Serializer<LegalityCheck>::Move
Serializer<LegalityCheck>::parse(const QString& str, int ysize,
				      const GameState& ref) {
  int offset = 0;
  return parse(str, offset, ysize, ref);
}

template <typename LegalityCheck>
typename Serializer<LegalityCheck>::Move
Serializer<LegalityCheck>::deserialize(const QString& str, const GameState& ref) {
  std::cerr << "shogi deserializing a " << qPrintable(m_rep) << " move" << std::endl;
  if (str[0].isDigit()) {
    // this is a move
    Point orig(ref.board().size().x - str[0].digitValue(), str[1].toAscii() - 'a');
    Point dest(ref.board().size().x - str[2].digitValue(), str[3].toAscii() - 'a');
    return Move(orig, dest, ((str.size() > 4) && (str[4] == '+')) ? 1 : -1);
  } 
  else
    return parse(str, ref.board().size().y, ref);
}

} // namespace Shogi
} // namespace HLVariant

#endif // HLVARIANT__SHOGI__SERIALIZER_H

