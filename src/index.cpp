/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include "index.h"

Index::operator QString() const {
  QString retv = QString::number(num_moves);

  for(int i = 0; i < (int)nested.size(); i++)
    retv += QString(",%1#%2").arg( nested[i].variation ).arg( nested[i].num_moves );

  return retv;
}

/** returns the number of moves from the start of the game */
int Index::totalNumMoves() const {
  int retv = num_moves;

  for(int i = 0; i < (int)nested.size(); i++)
    retv += nested[i].num_moves+1;

  return retv;
}

/** true if this index is the first of a variation branch */
bool Index::atVariationStart() const {
  return nested.size() && (nested[nested.size()-1].num_moves == 0);
}

/** flip variation */
Index Index::flipVariation(const Index& vstart, int v_id) const {
  int s = vstart.nested.size();
  if(s)
  {
    if( (int)nested.size() < s
        || vstart.num_moves != num_moves)
      return *this;

    for(int i=0;i<s-1;i++)
    if(vstart.nested[i] != nested[i])
      return *this;

    if(vstart.nested[s-1].variation != nested[s-1].variation
        || vstart.nested[s-1].num_moves > nested[s-1].num_moves)
      return *this;

    if(nested[s-1].num_moves > vstart.nested[s-1].num_moves) {
      Index retv(num_moves);
      for(int i=0;i<s;i++)
        retv.nested.push_back(vstart.nested[i]);
      retv.nested.push_back(Ref(v_id, nested[s-1].num_moves - vstart.nested[s-1].num_moves - 1));
      for(int i=s;i<(int)nested.size();i++)
        retv.nested.push_back(nested[i]);
      return retv;
    }
    else if(nested[s-1].num_moves == vstart.nested[s-1].num_moves
        && (int)nested.size() > s && nested[s].variation == v_id) {
      Index retv(num_moves);
      for(int i=0;i<s-1;i++)
        retv.nested.push_back(vstart.nested[i]);
      retv.nested.push_back(Ref(nested[s-1].variation, nested[s-1].num_moves + nested[s].num_moves + 1));
      for(int i=s+1;i<(int)nested.size();i++)
        retv.nested.push_back(nested[i]);
      return retv;
    }
    else
      return *this;
  }
  else if(num_moves > vstart.num_moves) {
    Index retv(vstart.num_moves);
    retv.nested.push_back(Ref(v_id,num_moves - vstart.num_moves - 1));
    for(int i=0;i<(int)nested.size();i++)
      retv.nested.push_back(nested[i]);
    return retv;
  }
  else if(num_moves == vstart.num_moves
      && nested.size() > 0 && nested[0].variation == v_id) {
    Index retv(num_moves + nested[0].num_moves + 1);
    for(int i=1;i<(int)nested.size();i++)
      retv.nested.push_back(nested[i]);
    return retv;
  }
  else
    return *this;
}

/** Returns an index the point to the next position. If variation is != -1, you will be entering in the
    sub-variation with this id instead of continuing in the current main line. You can also specify
    the number of moves you want to go on (in the main line or in the specified variation) */
Index Index::next(int variation_id, int num) const {
  Index retv = *this;
  if(variation_id != -1)
    retv.nested.push_back( Ref(variation_id, num-1) );
  else if(retv.nested.size() == 0)
    retv.num_moves += num;
  else
    retv.nested.rbegin()->num_moves += num;

  return retv;
}

/** Returns an index pointing to the previous position (or to a position n moves back)  */
Index Index::prev(int _num) const {
  int num = _num;
  Index retv = *this;

  while(num) {
    if(retv.nested.size() == 0) {
      if(retv.num_moves < num) {
        std::cout << "--> Error in Index::prev! Cannot rewind index " << *this <<
                              " by " << _num << "!" << std::endl;
        return Index(-1);
      }
      retv.num_moves -= num;
      num = 0;
    }
    else {
      if(retv.nested.rbegin()->num_moves >= num) {
        retv.nested.rbegin()->num_moves -= num;
        num = 0;
      }
      else {
        num -= retv.nested.rbegin()->num_moves+1;
        retv.nested.pop_back();
      }
    }
  }

  return retv;
}

/** Returns an index that is the minimum of this index and the given one, ie the branch point
    of the lines from start to the two indices. */
Index Index::min(const Index& ix) const  {
  if(ix.num_moves != num_moves)
    return Index( std::min(ix.num_moves, num_moves) );

  Index retv(num_moves);
  for(int i = 0; (i < (int)nested.size()) && (i < (int)ix.nested.size()); i++) {
    if(nested[i].variation != ix.nested[i].variation)
      break;
    retv.nested.push_back(Ref(nested[i].variation,
        std::min(ix.nested[i].num_moves, nested[i].num_moves) ));
    if(ix.nested[i].num_moves != nested[i].num_moves)
      break;
  }

  return retv;
}

/** Returns the number of steps down and up you have to do to go from this index to the given one */
std::pair<int, int> Index::stepsTo(const Index& ix) const {
  int i;
  int down = 0, up = 0;
  bool branch = ix.num_moves != num_moves;
  if(num_moves>ix.num_moves)
    down += num_moves-ix.num_moves;
  if(num_moves<ix.num_moves)
    up += ix.num_moves-num_moves;

  for(i = 0; (i < (int)nested.size()) && (i < (int)ix.nested.size()); i++) {
    if(nested[i].variation != ix.nested[i].variation)
      branch = true;
    if(branch) {
      down += nested[i].num_moves+1;
      up += ix.nested[i].num_moves+1;
      continue;
    }
    if(ix.nested[i].num_moves != nested[i].num_moves)
      branch = true;
    if(nested[i].num_moves>ix.nested[i].num_moves)
      down += nested[i].num_moves-ix.nested[i].num_moves;
    if(nested[i].num_moves<ix.nested[i].num_moves)
      up += ix.nested[i].num_moves-nested[i].num_moves;
  }
  for(; i<(int)nested.size();i++)
      down += nested[i].num_moves+1;
  for(; i<(int)ix.nested.size();i++)
      up += ix.nested[i].num_moves+1;
  return std::pair<int,int>(down, up);
}

/** returns the number of moves in the most nested variation */
int Index::lastIndex() {
  return nested.size() ? nested[nested.size()-1].num_moves : num_moves;
}
