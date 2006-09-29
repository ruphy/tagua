/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <map>
#ifdef Q_CC_MSVC
  #pragma warning( push )
  #pragma warning( disable : 4100 )
  #include <boost/variant.hpp>
  #pragma warning( pop )
#else
  #include <boost/variant.hpp>
#endif
#include "pgnparser.h"
#include "kboard.h"
#include "game.h"
#include "game_p.h"


using namespace GamePrivate;

/**
    \class Game game.h <game.h>
    \brief A game with history and variations.

    This template class encapsulates an editable game with history and undo editing.
*/

/** Constructor, creates an empty game*/
Game::Game()
: current(-1)
, undo_pos(0) {
}

/** destructor */
Game::~Game() {
}

void Game::onAdded(const Index&) {
}

void Game::onRemoved(const Index&) {
}

void Game::onEntryChanged(const Index&, int) {
}

void Game::onPromoteVariation(const Index&, int) {
}

void Game::onSetComment(const Index&, const QString&) {
}

void Game::onSetVComment(const Index&, int, const QString&) {
}

void Game::onCurrentIndexChanged(const Index&) {
}

void Game::onAvailableUndo(bool) {
}

void Game::onAvailableRedo(bool) {
}

Entry* Game::fetch(const Index& ix) {
  int at;
  History *vec = fetchRef(ix, &at);
  return vec ? &(*vec)[at] : NULL;
}

const Entry* Game::fetch(const Index& ix) const {
  int at;
  const History *vec = fetchRef(ix, &at);
  return vec ? &(*vec)[at] : NULL;
}

History* Game::fetchRef(const Index& ix, int* idx) {
  if(ix.num_moves >= (int)history.size() || ix.num_moves < 0 )
    return NULL;

  History* aretv = &history;
  Entry* retv = &history[ix.num_moves];
  if(idx) *idx = ix.num_moves;

  for(int i=0; i<(int)ix.nested.size();i++) {
    Variations::iterator it = retv->variations.find(ix.nested[i].variation);
    if(it == retv->variations.end() || ix.nested[i].num_moves >= (int)it->second.size()
                                || ix.nested[i].num_moves < 0 )
      return NULL;

    aretv = &it->second;
    retv = &it->second[ix.nested[i].num_moves];
    if(idx) *idx = ix.nested[i].num_moves;
  }
  return aretv;
}

const History* Game::fetchRef(const Index& ix, int* idx) const {
  return const_cast<const History*>(const_cast<Game*>(this)->fetchRef(ix, idx));
}

void Game::testMove(const Index& ix) {
  if (ix != Index(0)) {
    Entry *e1 = fetch(ix.prev());
    Entry *e2 = fetch(ix);
    if(!e1 || !e2 || !e1->position || !e2->move)
      return;

    if (!e1->position->testMove(e2->move)) {
      std::cout << "--> warning, invalid move added to game history!" << std::endl;
      //e2->move = MovePtr();
    }
  }
}

void Game::testMove() {
  testMove(current);
}

void Game::saveUndo(const UndoOp& op) {
  bool redo = undo_pos < (int)undo_history.size();

  while(undo_pos < (int)undo_history.size())
    undo_history.pop_back();
  undo_history.push_back(op);
  undo_pos++;

  if(undo_pos == 1)
    onAvailableUndo(true);
  if(redo)
    onAvailableRedo(false);
}


/** returns the index of the current position */
Index Game::index() const {
  return current;
}

/** returns an index opinting to the last position in the main line */
Index Game::lastMainlineIndex() const {
  return Index(history.size()-1);
}

/** true if the game contains the index */
bool Game::containsIndex(const Index& index) const {
  return !!fetch(index);
}

/** returns the current move */
MovePtr Game::move() const {
  return move(current);
}

/** returns the move at the given index */
MovePtr Game::move(const Index& index) const {
  Entry *e = (Entry*)fetch(index);
  if(!e) {
    std::cout << "--> Error in Game::position! Index out of range!" << std::endl;
    return MovePtr();
  }
  return e->move;
}

/** returns the current position */
PositionPtr Game::position() const {
  return position(current);
}

/** returns the position at the given index */
PositionPtr Game::position(const Index& index) const {
  Entry *e = (Entry*)fetch(index);
  if(!e) {
    std::cout << "--> Error in Game::position! Index out of range!" << std::endl;
    return PositionPtr();
  }
  return e->position;
}

/** returns the current comment */
QString Game::comment() const {
  return comment(current);
}

/** returns the comment at the given index */
QString Game::comment(const Index& index) const {
  const Entry *e = fetch(index);
  if(!e) {
    std::cout << "--> Error in Game::comment! Index out of range!" << std::endl;
    return QString();
  }
  return e->comment;
}

void Game::reset(PositionPtr pos) {
  Q_ASSERT(pos);

  undo_pos = 0;
  undo_history.clear();
  history.clear();
  history.push_back( Entry(MovePtr(), pos) );
  current = Index(0);
  onCurrentIndexChanged();
}

/** undo */
void Game::undo() {
  if(undo_pos <= 0) {
    std::cout << "--> Info: Cannot undo at the beginning of the undo history!" << std::endl;
    return;
  }

  bool last_undo = undo_pos == 1;
  bool now_redo = undo_pos == (int)undo_history.size();

  undo_pos--;
  UndoOp* op = &(undo_history[undo_pos]);

  if(boost::get<UndoAdd>(op)) {
    UndoAdd *a = boost::get<UndoAdd>(op);

    if(a->index.atVariationStart() ) {
      Entry* e = fetch(a->index.prev());
      Q_ASSERT(e);

      int v = a->index.nested[a->index.nested.size()-1].variation;
      Q_ASSERT(e->variations.count(v) == 1);
      Q_ASSERT(e->variations[v].size() == 1);

      e->variations.erase(v);
    }
    else {
      int at;
      std::vector<Entry>* vec = fetchRef(a->index, &at);
      Q_ASSERT(vec);
      Q_ASSERT((int)vec->size() == at+1);

      vec->pop_back();
    }

    if(current == a->index) {
      current = current.prev();
      onCurrentIndexChanged();
    }

    onRemoved(a->index);
  }
  else if(boost::get<UndoPromote>(op)) {
    UndoPromote *p = boost::get<UndoPromote>(op);

    int at;
    std::vector<Entry>* vec = fetchRef(p->index, &at);
    Q_ASSERT(vec);
    Q_ASSERT((*vec)[at].variations.count(p->variation)==1);

    History vold = (*vec)[at].variations[p->variation];
    History vnew;
    for(int i=at+1; i<(int)vec->size(); i++)
      vnew.push_back((*vec)[i]);
    while((int)vec->size()>at+1)
      vec->pop_back();
    for(int i=0; i<(int)vold.size(); i++)
      vec->push_back(vold[i]);
    (*vec)[at].variations[p->variation] = vnew;

    current = current.flipVariation(p->index, p->variation);
    onPromoteVariation(p->index, p->variation);
    //onCurrentIndexChanged();
  }
  else if(boost::get<UndoTruncate>(op)) {
    UndoTruncate *t = boost::get<UndoTruncate>(op);

    int at;
    std::vector<Entry>* vec = fetchRef(t->index, &at);
    Q_ASSERT(vec);
    Q_ASSERT((int)vec->size() == at+1);
    Q_ASSERT((*vec)[at].variations.empty());

    for(int i=0;i<(int)t->history.size();i++)
      vec->push_back(t->history[i]);
    (*vec)[at].variations = t->variations;
    (*vec)[at].vcomments = t->vcomments;

    if(t->history.size())
      onAdded(t->index.next());
    for(Variations::iterator it = t->variations.begin(); it != t->variations.end(); ++it)
      onAdded(t->index.next(it->first));
    for(VComments::iterator it = t->vcomments.begin(); it != t->vcomments.end(); ++it)
      onSetVComment(t->index, it->first, it->second);
  }
  else if(boost::get<UndoRemove>(op)) {
    UndoRemove *r = boost::get<UndoRemove>(op);

    Entry *e = fetch(r->index);
    e->variations[r->variation] = r->history;
    onAdded(r->index.next(r->variation));
    if(!r->vcomment.isEmpty()) {
      e->vcomments[r->variation] = r->vcomment;
      onSetVComment(r->index, r->variation, r->vcomment);
    }
  }
  else if(boost::get<UndoClear>(op)) {
    UndoClear *c = boost::get<UndoClear>(op);

    Entry *e = fetch(c->index);
    e->variations = c->variations;
    e->vcomments = c->vcomments;
    for(Variations::iterator it = c->variations.begin(); it != c->variations.end(); ++it)
      onAdded(c->index.next(it->first));
    for(VComments::iterator it = c->vcomments.begin(); it != c->vcomments.end(); ++it)
      onSetVComment(c->index, it->first, it->second);
  }
  else if(boost::get<UndoSetComment>(op)) {
    UndoSetComment *sc = boost::get<UndoSetComment>(op);
    Entry *e = fetch(sc->index);
    Q_ASSERT(e);

    if(sc->variation == -1) {
      e->comment = sc->old_comment;
      onSetComment(sc->index, sc->old_comment);
    }
    else {
      if(sc->old_comment.isEmpty())
        e->vcomments.erase(sc->variation);
      else
        e->vcomments[sc->variation] = sc->old_comment;
      onSetVComment(sc->index, sc->variation, sc->old_comment);
    }
  }

  if(last_undo)
    onAvailableUndo(false);
  if(now_redo)
    onAvailableRedo(true);
}

/** redo */
void Game::redo() {
  if(undo_pos >= (int)undo_history.size()) {
    std::cout << "--> Info: Cannot redo at the end of the undo history!" << std::endl;
    return;
  }

  bool now_undo = undo_pos == 0;
  bool last_redo = undo_pos == (int)undo_history.size()-1;

  UndoOp* op = &(undo_history[undo_pos]);
  undo_pos++;

  if(boost::get<UndoAdd>(op)) {
    UndoAdd *a = boost::get<UndoAdd>(op);

    if(a->index.atVariationStart() ) {
      Entry* e = fetch(a->index.prev());
      Q_ASSERT(e);

      int v = a->index.nested[a->index.nested.size()-1].variation;
      Q_ASSERT(e->variations.count(v) == 0);

      History h;
      h.push_back(a->entry);
      e->variations[v] = h;
    }
    else {
      int at;
      std::vector<Entry>* vec = fetchRef(a->index.prev(), &at);
      Q_ASSERT(vec);
      Q_ASSERT((int)vec->size() == at+1);

      vec->push_back(a->entry);
    }

    onAdded(a->index);
  }
  else if(boost::get<UndoPromote>(op)) {
    UndoPromote *p = boost::get<UndoPromote>(op);

    int at;
    std::vector<Entry>* vec = fetchRef(p->index, &at);

    Q_ASSERT(vec);
    Q_ASSERT((*vec)[at].variations.count(p->variation)==1);
    History vold = (*vec)[at].variations[p->variation];
    History vnew;
    for(int i=at+1; i<(int)vec->size(); i++)
      vnew.push_back((*vec)[i]);
    while((int)vec->size()>at+1)
      vec->pop_back();
    for(int i=0; i<(int)vold.size(); i++)
      vec->push_back(vold[i]);
    (*vec)[at].variations[p->variation] = vnew;

    current = current.flipVariation(p->index, p->variation);
    onPromoteVariation(p->index, p->variation);
    //onCurrentIndexChanged();
  }
  else if(boost::get<UndoTruncate>(op)) {
    UndoTruncate *t = boost::get<UndoTruncate>(op);

    int at;
    std::vector<Entry>* vec = fetchRef(t->index, &at);
    Q_ASSERT(vec);
    Q_ASSERT((int)vec->size() == at+1+(int)t->history.size());

    while((int)vec->size() > at+1)
      vec->pop_back();
    (*vec)[at].variations.clear();
    (*vec)[at].vcomments.clear();

    if(current > t->index) {
      current = t->index;
      onCurrentIndexChanged();
    }

    if(t->history.size())
      onRemoved(t->index.next());
    for(Variations::iterator it = t->variations.begin(); it != t->variations.end(); ++it)
      onRemoved(t->index.next(it->first));
  }
  else if(boost::get<UndoRemove>(op)) {
    UndoRemove *r = boost::get<UndoRemove>(op);

    Entry *e = fetch(r->index);
    e->variations.erase(r->variation);
    e->vcomments.erase(r->variation);
    onRemoved(r->index.next(r->variation));
  }
  else if(boost::get<UndoClear>(op)) {
    UndoClear *c = boost::get<UndoClear>(op);

    Entry *e = fetch(c->index);
    e->variations.clear();
    e->vcomments.clear();
    for(Variations::iterator it = c->variations.begin(); it != c->variations.end(); ++it)
      onRemoved(c->index.next(it->first));
  }
  else if(boost::get<UndoSetComment>(op)) {
    UndoSetComment *sc = boost::get<UndoSetComment>(op);
    Entry *e = fetch(sc->index);
    Q_ASSERT(e);

    if(sc->variation == -1) {
      e->comment = sc->new_comment;
      onSetComment(sc->index, sc->new_comment);
    }
    else {
      if(sc->new_comment.isEmpty())
        e->vcomments.erase(sc->variation);
      else
        e->vcomments[sc->variation] = sc->new_comment;
      onSetVComment(sc->index, sc->variation, sc->new_comment);
    }
  }

  if(now_undo)
    onAvailableUndo(true);
  if(last_redo)
    onAvailableRedo(false);
}

/** sets the comment in the current index */
void Game::setComment(const QString& c) {
  setComment(current, c);
}

/** sets the comment in the given index */
void Game::setComment(const Index& ix, const QString& c) {
  Entry* e = fetch(ix);
  if(!e) {
    std::cout << "--> Error in Game::setComment! Invalid index!" << std::endl;
    return;
  }
  if(e->comment == c)
    return;

  saveUndo(UndoSetComment(ix, -1, e->comment, c));
  e->comment = c;
  onSetComment(ix, c);
}

/** sets the variation comment in the given index/variation */
void Game::setVComment(const Index& ix, int v, const QString& c) {
  Entry* e = fetch(ix);
  if(!e) {
    std::cout << "--> Error in Game::setComment! Invalid index!" << std::endl;
    return;
  }
  QString oc = e->vcomments.count(v) ? e->vcomments[v] : QString();
  if(oc == c)
    return;

  saveUndo(UndoSetComment(ix, v, oc, c));
  if(c.isEmpty())
    e->vcomments.erase(v);
  else
    e->vcomments[v] = c;
  onSetVComment(ix, v, c);
}

/** promotes the current position in the upper main line */
void Game::promoteVariation() {
  promoteVariation(current);
}

/** promotes the given position in the upper main line */
void Game::promoteVariation(const Index& _ix) {
  if(_ix.nested.size()==0) {
    std::cout << "--> Error in Game::promoteVariation! cannot promote main line!" << std::endl;
    return;
  }
  Index ix = _ix;
  int v = ix.nested[ix.nested.size()-1].variation;
  ix.nested.pop_back();

  promoteVariation(ix, v);
}

/** promotes the given variation in the upper main line */
void Game::promoteVariation(const Index& ix, int v) {
  int at;
  std::vector<Entry>* vec = fetchRef(ix, &at);
  Q_ASSERT(vec);
  Q_ASSERT((*vec)[at].variations.count(v)==1);

  History vold = (*vec)[at].variations[v];
  History vnew;
  for(int i=at+1; i<(int)vec->size(); i++)
    vnew.push_back((*vec)[i]);
  while((int)vec->size()>at+1)
    vec->pop_back();
  for(int i=0; i<(int)vold.size(); i++)
    vec->push_back(vold[i]);
  (*vec)[at].variations[v] = vnew;

  saveUndo(UndoPromote(ix, v));
  current = current.flipVariation(ix, v);
  onPromoteVariation(ix, v);
  //don't call onCurrentIndexChanged(), as the position did not change actually
}

/** removes the given variation in the current index */
void Game::removeVariation(int v) {
  removeVariation(current, v);
}

/** removes the given variation in the given index */
void Game::removeVariation(const Index& _ix) {
  if(_ix.nested.size()==0) {
    std::cout << "--> Error in Game::removeVariation! cannot remove main line!" << std::endl;
    return;
  }
  Index ix = _ix;
  int v = ix.nested[ix.nested.size()-1].variation;
  ix.nested.pop_back();

  removeVariation(ix, v);
}

/** removes the given variation in the given index */
void Game::removeVariation(const Index& ix, int v) {
  Entry* e = fetch(ix);

  saveUndo(UndoRemove(ix, v, e->variations[v],
                e->vcomments.count(v) ? e->vcomments[v] : QString() ));
  e->variations.erase(v);
  e->vcomments.erase(v);

  onRemoved(ix.next(v));
  if(current >= ix.next(v)) {
    current = ix;
    onCurrentIndexChanged();
  }
}

/** removes the given variation in the current index */
void Game::clearVariations() {
  clearVariations(current);
}

/** removes the given variation in the given index */
void Game::clearVariations(const Index& ix) {
  Entry* e = fetch(ix);

  UndoClear uc(ix, e->variations, e->vcomments);
  saveUndo(uc);
  e->variations.clear();
  e->vcomments.clear();

  for(Variations::iterator it = uc.variations.begin(); it != uc.variations.end(); ++it)
    onRemoved(ix.next(it->first));
  if(current > ix && !(current >= ix.next())) {
    current = ix;
    onCurrentIndexChanged();
  }
}

/** removes all the successors of the current position */
void Game::truncate() {
  truncate(current);
}

/** removes all the successors of the given position */
void Game::truncate(const Index& ix) {
  int at;
  History* vec = fetchRef(ix, &at);
  if(!vec) {
    std::cout << "--> Error in Game::truncate! Truncating at an unexisting index!" << std::endl;
    return;
  }

  Entry *e = &(*vec)[at];
  UndoTruncate undo(ix);
  for(int i=at+1; i<(int)vec->size();i++)
    undo.history.push_back((*vec)[i]);
  while((int)vec->size()>at+1)
    vec->pop_back();

  undo.variations = e->variations;
  undo.vcomments = e->vcomments;
  saveUndo(undo);
  e->variations.clear();
  e->vcomments.clear();

  if(undo.history.size())
    onRemoved(undo.index.next());
  for(Variations::iterator it = undo.variations.begin(); it != undo.variations.end(); ++it)
    onRemoved(undo.index.next(it->first));

  if(current > ix) {
    current = ix;
    onCurrentIndexChanged();
  }
}

/** adds a new move+position after the current one, on the main
    line if possible, or else in a new variation */
void Game::add(MovePtr m, PositionPtr pos) {
  Q_ASSERT(pos);

  Index old_c = current;
  int at;
  std::vector<Entry>* vec = fetchRef(current, &at);
  Q_ASSERT(vec);

  /* add the move on the mainline */
  if((int)vec->size() <= at+1 ) {
    Q_ASSERT((int)vec->size() == at+1);
    vec->push_back(Entry(m, pos));
    current = current.next();
    testMove();
    saveUndo(UndoAdd(current, Entry(m, pos)));
    onAdded(current);
    onCurrentIndexChanged(old_c);
  }
  /* we are playing the move that is already next in the mainline */
  else if( (*vec)[at+1].position && (*vec)[at+1].position->equal(pos)) {
    current = current.next();
    onCurrentIndexChanged(old_c);
    /* no need to test the move */
  }
  else {
    Entry *e = fetch(current);
    Q_ASSERT(e);

    /* check if a variations with this move already exists. */
    for(Variations::iterator it = e->variations.begin(); it != e->variations.end(); ++it)
    if(it->second.size() > 0 && it->second[0].position
        && it->second[0].position->equal(pos) ) {
      current = current.next(it->first);
      onCurrentIndexChanged(old_c);

      return;
    }

    int var_id = e->last_var_id++;
    e->variations[var_id].push_back(Entry(m, pos));
    current = current.next(var_id);
    testMove();
    saveUndo(UndoAdd(current, Entry(m, pos)));
    onAdded(current);
    onCurrentIndexChanged(old_c);
  }
}

/** forces a move+position at in certain index */
bool Game::insert(MovePtr m, PositionPtr pos, const Index& at) {
  Entry *e = fetch(at);

  if(!e) {
    if(at.nested.size() == 0) {
      if(undo_history.size()) {
        undo_pos = 0;
        undo_history.clear();
      }
      int hs = history.size();
      history.resize(at.num_moves + 1);
      history[at.num_moves] = Entry(m, pos);
      testMove(at);
      onAdded(Index(hs));
      return true;
    }
    else {
      std::cout << "--> Error in Game::insert! Index out of range!" << std::endl;
      return false;
    }
  }

  if(undo_history.size()) {
    undo_pos = 0;
    undo_history.clear();
  }
  bool res = e->position && e->position->equal(pos);
  //*e = Entry(m, pos);
  e->move = m;
  e->position = pos;
  testMove(at);
  testMove(at.next());
  for (Variations::const_iterator it = e->variations.begin();
          it != e->variations.end(); ++it)
    testMove(at.next(it->first));
  onEntryChanged(at);
  return res;
}

/** returns true if we cannot go forward */
bool Game::lastPosition() const {
  return !fetch(current.next());
}

/** go back */
bool Game::back() {
  if (current <= 0) return false; // first entry or uninitialized
  Index old_c = current;
  Index new_c = current.prev();

  Entry *e = fetch(new_c);
  if(!e || e->position == 0) return false; // gap immediately before current
  current = new_c;
  onCurrentIndexChanged(old_c);

  return true;
}

/** go forward (in the current mainline) */
bool Game::forward() {
  Index old_c = current;
  Index new_c = current.next();

  Entry *e = fetch(new_c);
  if(!e || e->position == 0) {
     return false; // gap immediately before current
  }
  current = new_c;
  onCurrentIndexChanged(old_c);

  return true;
}

/** go to the root position */
void Game::gotoFirst() {
  Index old_c = current;
  current = Index(0);
  onCurrentIndexChanged(old_c);
}

/** go to the last position (in the current mainline) */
void Game::gotoLast() {
  int at;
  std::vector<Entry>* vec = fetchRef(current, &at);
  Q_ASSERT(vec);
  Q_ASSERT((int)vec->size() > at);

  if((int)vec->size() > at+1) {
    Index old_c = current;
    current = current.next(-1, vec->size()-1-at);
    onCurrentIndexChanged(old_c);
  }
}

/** go to a specified index */
bool Game::goTo(const Index& index) {
  if (fetch(index)) {
    Index old_c = current;
    current = index;
    onCurrentIndexChanged(old_c);
    return true;
  }
  return false;
}

QString Game::variationPgn(const History& vec, const Entry& e,
                        int start, const Index& _ix) const {
  Index ix = _ix;
  QString res;

  for (int i = start; i < static_cast<int>(vec.size()); i++) {
    const Entry& preve = (i > start) ? vec[i-1] : e;

    QString mv = (vec[i].move && preve.position) ?
              vec[i].move->SAN( preve.position ) : "???";
#if 0
    if (ix == current)
      mv = "[[" + mv + "]]";
#endif

    int n = ix.totalNumMoves()+1;
    if(i==start || n%2==0)
      mv = QString::number(n/2)+(n%2==1 ? ". ... " : ". ") + mv;
    if (i > start)
      mv = " " + mv;

    res += mv;

    if(!vec[i].comment.isEmpty())
      res += " {" + vec[i].comment + "}";

    if(i > 0) {
      for(Variations::const_iterator it = vec[i-1].variations.begin();
             it != vec[i-1].variations.end(); ++it) {
        res += " (";
        if(vec[i-1].vcomments.count(it->first))
          res += "{" + vec[i-1].vcomments.find(it->first)->second + "} ";
        res += variationPgn(it->second, vec[i - 1], 0,
                    ix.prev().next(it->first)) + ")";
      }
    }

    ix = ix.next();
  }
  return res;
}

/** returns a pgn containing the whole game (with variations) */
QString Game::pgn() const {
  return variationPgn(history, history[0], 1, Index(1));
}

/** loads a pgn in the current game */
void Game::load(const PGN& pgn) {
  load(position(Index(0)), pgn);
}

/** loads a pgn in the current game */
void Game::load(PositionPtr pos, const PGN& pgn) {
  current = Index(0);
  undo_history.clear();

  Entry* fe = fetch(Index(0));
  int old_history_size = history.size();
  std::vector<int> v_ids;

  while(history.size()>1)
    history.pop_back();
  for(Variations::const_iterator it = fe->variations.begin();
          it != fe->variations.end(); ++it)
    v_ids.push_back(it->first);
  fe->variations.clear();
  fe->vcomments.clear();

  for(int i=0;i<(int)v_ids.size();i++)
    onRemoved(Index(0).next(v_ids[i]));
  if(old_history_size>1)
    onRemoved(Index(1));
  v_ids.clear();
  history[0].position = pos;

  QString vcomment;
  std::vector<Index> var_stack;
  bool var_start = false;

  for (uint i = 0; i < pgn.m_entries.size(); i++) {
    if(boost::get<QString>(pgn[i])) {
      if(var_start)
        vcomment += *boost::get<QString>(pgn[i]);
      else {
        Entry *e = fetch(current);
        Q_ASSERT(e);

        e->comment += *boost::get<QString>(pgn[i]);
      }
    }
    else if(boost::get<PGN::BeginVariation>(pgn[i])) {
      var_stack.push_back(current);
      var_start = true;
    }
    else if(boost::get<PGN::EndVariation>(pgn[i])) {
      if(var_stack.size() == 0) {
        std::cout << " --> Error, unexpected end variation!!!" << std::endl;
        break;
      }
      current = var_stack[var_stack.size()-1];
      var_stack.pop_back();
    }
    else if(boost::get<PGN::Move>(pgn[i])) {
      const PGN::Move *pm = boost::get<PGN::Move>(pgn[i]);

      int n = current.totalNumMoves()+1;
      if(var_start) {
        if(!pm->number)
          current = current.prev();
        else if(pm->number>n+1)
          std::cout << " --> Error, too far variation!!!" << std::endl;
        else {
          if(pm->number<n)
            std::cout << " --> Warning, too near variation..." << std::endl;
          current = current.prev(n + 1 - pm->number);
        }
      }
      else if(pm->number && pm->number!=n+1)
        std::cout << " --> Warning, move number mismatch..." << std::endl;

      PositionPtr pos = position();
      MovePtr m = pos->getMove(*pm);

      //std::cout << "processed " << pm->from.toString(8) << pm->to.toString(8)
        //                  << " = " << m.get() << std::endl;
      if(!m || !pos->testMove(m))
        break;

      PositionPtr newPos = pos->clone();
      newPos->move(m);

      int at;
      History *vec = fetchRef(current, &at);
      Q_ASSERT(vec);

      if(var_start) {
        Entry *e = &(*vec)[at];
        int var_id = e->last_var_id++;
        e->variations[var_id].push_back(Entry(m, newPos));
        if(!vcomment.isEmpty()) {
          e->vcomments[var_id] = vcomment;
          vcomment = QString();
        }
        /* this is a hack, but the mainline should NEVER
            be empty if there is a variation*/
        if((int)vec->size() - 1 == at)
          vec->push_back(Entry(m, newPos));

        current = current.next(var_id);
      }
      else {
        if((int)vec->size() - 1 == at)
          vec->push_back(Entry(m, newPos));
        else
          (*vec)[at] = Entry(m, newPos);

        current = current.next();
      }

      var_start = false;
    }
  }

  if(history.size()>1)
    onAdded(Index(1));
  Entry* e = fetch(Index(0));
  for(Variations::const_iterator it = e->variations.begin();
          it != e->variations.end(); ++it)
    onAdded(Index(0).next(it->first));
  for(VComments::const_iterator it = e->vcomments.begin();
          it != e->vcomments.end(); ++it)
    onSetVComment(Index(0), it->first, it->second);

  current = Index(0);
  onCurrentIndexChanged();
}
