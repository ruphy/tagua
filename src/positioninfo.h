/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef POSITIONINFO_H
#define POSITIONINFO_H

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <QRegExp>

#include "tagua.h"
#include "icsgamedata.h"
#include "fwd.h"

class PositionInfo {
public:
  enum Relation {
    MoveListStart     = -4,
    IsolatedPosition  = -3,
    ObservingExamined = -2,
    NotMyMove         = -1,
    ObservingPlayed   = 0,
    MyMove            = 1,
    Examining         = 2,
    UnknownRelation   = -255
  };

  class CaptureIndexes {
  public:
    enum {
      ChessboardStart   = 1,
      Turn              = 9,
      EnPassant         = 10,
      WhiteKingCastle   = 11,
      WhiteQueenCastle  = 12,
      BlackKingCastle   = 13,
      BlackQueenCastle  = 14,
      ReversibleMoves   = 15,
      GameNumber        = 16,
      WhitePlayer       = 17,
      BlackPlayer       = 18,
      Relation          = 19,
      StartingTime      = 20,
      StartingIncrement = 21,
      WhiteTime         = 24,
      BlackTime         = 25,
      MoveOrdinal       = 26,
      LastMoveVerbose   = 27,
      TimeUsed          = 28,
      LastMove          = 29,
      Flip              = 30
    };
  };

  class PositionRow {
  public:
    std::vector<AbstractPiece::Ptr> row;
    explicit PositionRow(const ICSAPIPtr& icsapi, const QString& str);
  };

  static QRegExp pattern;

  bool valid;
  PositionInfo();
  
  /** 
    * Parse a style12 line filling the fields of this PositionInfo object.
    * \param games Games we know about.
    * \param style12 A style12 line.
    * \return Whether the game is new.
    */
  bool load(std::map<int, ICSGameData>& games, const QString& style12);

  int turn;
  int moveIndex;
  int index() const;
  int gameNumber;
  QString whitePlayer;
  QString blackPlayer;
  AbstractPosition::Ptr position;
  Point enPassantSquare;

  Relation relation;

  QString lastMoveSAN;
  QString lastMove;
  int whiteTime;
  int blackTime;
};

#endif // POSITIONINFO_H
