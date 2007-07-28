/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef INDEX_H
#define INDEX_H

#include <QVector>
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
    Ref() : variation(-1), num_moves(-1) {}
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
  QVector<Ref> nested;

  /** Constructor, you can contruct it from an integer that
      is the number of moves played in the main line */
  Index(int n = -1) : num_moves(n) {}

  /** return a string like "1_2.3_4.5" */
  operator QString() const;

  /** returns the number of moves from the start of the game */
  int totalNumMoves() const;

  /** \return true if this index is the first of a variation branch */
  bool atVariationStart() const;

  /** flip variation: returns the current index changed as if it was in the subvariation \a vid
      at \a vstart instead of mainline and viceversa, ie at \a vstart the mainline becames
      subvariation \a vid and viceversa */
  Index flipVariation(const Index& vstart, int v_id) const;

  /** Returns an index the point to the next position. If variation is != -1, you will be entering in the
    sub-variation with this id instead of continuing in the current main line. You can also specify
    the number of moves you want to go on (in the main line or in the specified variation) */
  Index next(int variation_id = -1, int num = 1) const;

  /** Returns an index pointing to the previous position (or to a position n moves back)  */
  Index prev(int _num = 1) const;

  /** Returns an index that is the minimum of this index and the given one, ie the branch point
    of the lines from start to the two indices. */
  Index min(const Index& ix) const;

  /** Returns the number of steps down and up you have to do to go from this index to the given one */
  std::pair<int, int> stepsTo(const Index& ix) const;

  /** returns the number of moves in the most nested variation */
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

  /** \return an index from a string like "1_2.3_4.5" */
  static Index fromString(const QString& s);
  
  /** \return whether this index points to the mainline */
  bool mainLine() const;
};

#endif //INDEX_H
