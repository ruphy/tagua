/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef __INDEX_H__
#define __INDEX_H__

#include <vector>
#include <QString>
#include "common.h"

/**
    \class Index index.h <index.h>
    \brief An index in a multi-variations game.

    This is a class that you can use to refer to a position in a game.
*/
class Index {
public:
  class Ref {
  public:
    Ref(int v, int n) : variation(v), num_moves(n) {}
    int variation;
    int num_moves;

    bool operator==(const Ref& r) const {
      return r.variation == variation
              && r.num_moves == num_moves;
    }
    bool operator!=(const Ref& r) const { return !((*this)==r); }
  };

  int num_moves;
  std::vector<Ref> nested;

  /** Constructor, you can contruct it from an integer that
      is the number of moves played in the main line */
  Index(int n) : num_moves(n) {}

  operator QString() const;
  int totalNumMoves() const;
  bool atVariationStart() const;
  Index flipVariation(const Index& vstart, int v_id) const;
  Index next(int variation_id = -1, int num = 1) const;
  Index prev(int _num = 1) const;
  Index min(const Index& ix) const;
  std::pair<int, int> stepsTo(const Index& ix) const;
  int lastIndex();

  /** True if this index refers to a position 'before' than the given one */
  bool operator<(const Index& ix) const {
    std::pair<int,int> s = stepsTo(ix);
    return s.first == 0 && s.second>0;
  }

  /** True if this index refers to a position 'before or equal' than the given one */
  bool operator<=(const Index& ix) const {
    std::pair<int,int> s = stepsTo(ix);
    return s.first == 0;
  }

  /** True if this index refers to a position 'after' than the given one */
  bool operator>(const Index& ix) const {
    return ix < *this;
  }

  /** True if this index refers to a position 'after or equal' than the given one */
  bool operator>=(const Index& ix) const {
    return ix <= *this;
  }

  /** True if this index refers to the same position of the given one */
  bool operator==(const Index& ix) const {
    std::pair<int,int> s = stepsTo(ix);
    return s.first == 0 && s.second == 0;
  }

  /** True if this index refers to a different position of the given one */
  bool operator!=(const Index& ix) const {
    return !(*this == ix);
  }
};

#endif //__INDEX_H__
