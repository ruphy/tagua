/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef SERVERINFO_H
#define SERVERINFO_H

#include <boost/shared_ptr.hpp>
#ifdef Q_CC_MSVC
  #pragma warning( push )
  #pragma warning( disable : 4100 )
  #include <boost/variant.hpp>
  #pragma warning( pop )
#else
  #include <boost/variant.hpp>
#endif
#include "varianttraits.h"


template <typename Pos>
class ChessServerInfo {
public:
  boost::shared_ptr<Pos> position;
  ChessServerInfo(const boost::shared_ptr<Pos> position)
  : position(position) { }
};

template <typename Pos>
class PoolServerInfo {
public:
  typename PoolArray<
    typename PoolTraits<Pos>::type,
    PoolTraits<Pos>::number>::type pools;
};

template <typename Pos>
struct ServerInfo {
  typedef boost::variant<ChessServerInfo<Pos>, PoolServerInfo<Pos> > type;
};

#if 0


template <typename Pos> class ServerInfoVisitor;

template <typename Pos>
class ServerInfo {
public:
  virtual void apply(ServerInfoVisitor<Pos>& visitor) const = 0;
};

template<typename Pos> class ChessServerInfo;
template<typename Pos> class PoolServerInfo;

template <typename Pos>
class ServerInfoVisitor {
public:
  virtual void visit(const ChessServerInfo<Pos>& info) = 0;
  virtual void visit(const PoolServerInfo<Pos>& info) = 0;
};


template <typename Pos>
class ChessServerInfo : public ServerInfo<Pos> {
public:
  boost::shared_ptr<Pos> position;
  virtual void apply(ServerInfoVisitor<Pos>& visitor) const {
    visitor.visit(*this);
  }
  ChessServerInfo(const boost::shared_ptr<Pos> position)
  : position(position) { }
};

template <typename Pos>
class PoolServerInfo : public ServerInfo<Pos> {
public:
  typename PoolTraits<Pos>::type pools[PoolTraits<Pos>::number];
  virtual void apply(ServerInfoVisitor<Pos>& visitor) const {
    visitor.visit(*this);
  }
};

#endif // 0

#endif // SERVERINFO_H
