/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ICSAPI_H
#define ICSAPI_H

#include "tagua.h"

class ICSAPI {
public:
  virtual ~ICSAPI() { }
  
  /**
    * Create a position using the information of a style12 line.
    * \param turn Current turn.
    * \param wkCastle King-side castling is possible for white
    * \param wqCastle Queen-side castling is possible for white
    * \param bkCastle King-side castling is possible for black
    * \param bqCastle Queen-side castling is possible for black
    * \param ep En-passant square.
    */
  virtual PositionPtr createChessboard(
                int turn,
                bool wkCastle,
                bool wqCastle,
                bool bkCastle,
                bool bqCastle,
                const Point& ep) = 0;
                

  /**
    * Create a new piece using the given description string.
    * \param description A string representing the piece to be created. Its
    *                     meaning is defined by the variant.
    * \return A newly created piece.
    */
  virtual PiecePtr createPiece(const QString& description) = 0;
  
  /**
    * Parse ICS verbose notation.
    * \param str A string representation of the move to be parsed.
    * \param turn The player who is moving.
    * \return A move object corresponding to the given verbose representation.
    */
  virtual MovePtr parseVerbose(const QString& str, const PositionPtr& ref) = 0;
};

// wrapper

template <typename Variant>
class WrappedICSAPI : public ICSAPI {
public:
  virtual PositionPtr createChessboard(int, bool, bool, bool, bool, const Point&);
  virtual PiecePtr createPiece(const QString& description);
  virtual MovePtr parseVerbose(const QString&, const PositionPtr&) { return MovePtr(); }
};

#endif // ICSAPI_H

