/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "positioninfo.h"
#include "variants.h"
#include "gameinfo.h"
#include "icsapi.h"

using namespace boost;

// Style 12 was designed by Daniel Sleator (sleator+@cs.cmu.edu) Darooha@ICC
QRegExp PositionInfo::pattern(
                                "^<12>\\s+"                          //header
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([qkbnrpQKBNRP-]{8})\\s+"           //chessboard
                                "([BW])\\s+"                         //black/white
                                "(-1|[0-7])\\s+"                     //passing pawn
                                "([01])\\s+"                         //castle rights
                                "([01])\\s+"                         //castle rights
                                "([01])\\s+"                         //castle rights
                                "([01])\\s+"                         //castle rights
                                "(-?\\d+)\\s+"                       //50 moves made
                                "(\\d+)\\s+"                         //game num
                                "(\\S+)\\s+"                         //white name
                                "(\\S+)\\s+"                         //black name
                                "(-[1-4]|[0-2])\\s+"                 //status
                                "(\\d+)\\s+"                         //time
                                "(\\d+)\\s+"                         //inc
                                "(\\d+)\\s+"                         //w material
                                "(\\d+)\\s+"                         //b material
                                "(-?\\d+)\\s+"                       //w time
                                "(-?\\d+)\\s+"                       //b time
                                "(\\d+)\\s+"                         //move made
                                "(\\S+)\\s+"                         //coordmove
                                "(\\S+)\\s+"                         //time used
                                "(\\S+)\\s+"                         //algmove
                                "([0-1])"                            //flip

);

PositionInfo::PositionRow::PositionRow(const ICSAPIPtr& icsapi, const QString& str) {
  Q_ASSERT(str.length() == 8);

  row.resize(str.length());
  for (int i = 0; i < str.length(); ++i) {
    QChar c = str[i];
    
    row[i] = icsapi->createPiece(c);
  }
}

/**
  * @return Position index as a 0-based halfmove count.
 */
int PositionInfo::index() const {
  int res = (moveIndex - 1) * 2;
  if (turn == 1) res++;
  return res;
}

PositionInfo::PositionInfo(const std::map<int, ICSGameData>& games, const QString& str) {
  if (pattern.indexIn(str) != 0) {
    valid = false;
    return;
  }

  valid = true;
  int gn = pattern.cap(CaptureIndexes::GameNumber).toInt();
  std::map<int, ICSGameData>::const_iterator gi = games.find(gn);
  ICSAPIPtr icsapi;
  
  if (gi == games.end()) {
    ERROR("Received style12 for unknown game  " << gn);
    icsapi = Variant::variant("Dummy")->icsAPI();
  }
  else {
    icsapi = gi->second.icsapi;
  }

  std::vector<PositionRow> rows;
  for (uint i = 0; i < 8; ++i)
    rows.push_back(PositionRow(icsapi, pattern.cap(CaptureIndexes::ChessboardStart + i)));

  gameNumber = pattern.cap(CaptureIndexes::GameNumber).toInt();
  moveIndex = pattern.cap(CaptureIndexes::MoveOrdinal).toInt();
  whitePlayer = pattern.cap(CaptureIndexes::WhitePlayer);
  blackPlayer = pattern.cap(CaptureIndexes::BlackPlayer);
  turn = pattern.cap(CaptureIndexes::Turn) == "W"? 0 : 1;

  int ep = pattern.cap(CaptureIndexes::EnPassant).toInt();
  if (ep == -1)
    enPassantSquare = Point::invalid();
  else
    enPassantSquare = Point(ep, turn == 0? 2 : 5);

  bool wkCastle = pattern.cap(CaptureIndexes::WhiteKingCastle).toInt() == 1;
  bool wqCastle = pattern.cap(CaptureIndexes::WhiteQueenCastle).toInt() == 1;
  bool bkCastle = pattern.cap(CaptureIndexes::BlackKingCastle).toInt() == 1;
  bool bqCastle = pattern.cap(CaptureIndexes::BlackQueenCastle).toInt() == 1;

  position = icsapi->createChessboard(turn, wkCastle, wqCastle, bkCastle, bqCastle, enPassantSquare);
  for (uint i = 0; i < 8; ++i) {
    for (uint j = 0; j < rows[i].row.size(); ++j) {
      position->set(Point(j,i), rows[i].row[j]);
    }
  }

  relation = static_cast<Relation>(pattern.cap(CaptureIndexes::Relation).toInt());

  whiteTime = pattern.cap(CaptureIndexes::WhiteTime).toInt();
  blackTime = pattern.cap(CaptureIndexes::BlackTime).toInt();
  if (pattern.cap(CaptureIndexes::TimeUsed).indexOf('.') == -1) {
    // time is in seconds
    whiteTime *= 1000;
    blackTime *= 1000;
  }

  lastMoveSAN = pattern.cap(CaptureIndexes::LastMove);
  lastMove = pattern.cap(CaptureIndexes::LastMoveVerbose);
}
