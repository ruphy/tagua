/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CHESSREFERENCE_H
#define CHESSREFERENCE_H

#include <boost/shared_ptr.hpp>
#include "serverinfo.h"

template <typename Pos>
struct ComparePosition {
  static bool apply(const Pos& chessPos, const Pos& pos) {
    return static_cast<const ChessPosition&>(chessPos) == pos;
  }
};

/**
  * @brief Common base for ServerReference specializations.
  */
template <typename Pos>
struct ServerReferenceBase {
  typedef Pos Position;
  
  class InfoVisitor : public boost::static_visitor<bool> {
    const Position& m_position;
  public:
    InfoVisitor(const Position& position)
    : m_position(position) { }
    bool operator()(const ChessServerInfo<Pos>& info) const {
      return ComparePosition<Pos>::apply(
        *info.position, 
        m_position);
    }
    bool operator()(const PoolServerInfo<Pos>&) const {
      return true; // FIXME
    }    
  };
  
  /**
    * Compare a position with a @a ChessPosition. Used to check locally
    * computed positions against server provided positions.
    */
  static bool checkPosition(const Position& pos, const ChessPosition& serverPos) {
    return serverPos == pos;
  }
  
  /**
    * Retrieve a position shared pointer from a @a ChessPosition
    * pointer. Used to retrieve variant specific position
    * information from @a style12.position
    */
  static boost::shared_ptr<Position> getPosition(
              const boost::shared_ptr<ChessPosition>& pos) {
    return boost::shared_ptr<Position>(
      new Position(*pos));
  }
  
  
  /**
    * Compare a position with partial information contained in another position.
    */
  static bool checkPartialPosition(const Position& pos, const typename ServerInfo<Position>::type& server) {
    return boost::apply_visitor(InfoVisitor(pos), server);
  }
};

/**
  * @brief ServerReference is used whenever information coming from
  *        the server has to be compared with variant specific structures.
  */
template <typename Pos>
struct ServerReference : public ServerReferenceBase<Pos> { };

template <>
struct ServerReference<ChessPosition> 
: public ServerReferenceBase<ChessPosition> {
  static boost::shared_ptr<ChessPosition> getPosition(
      const boost::shared_ptr<ChessPosition>& pos) {
    return pos;
  }
};

#endif // CHESSREFERENCE_H
