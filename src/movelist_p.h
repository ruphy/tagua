/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVELIST_P_H
#define MOVELIST_P_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <QFont>
#include <QFontMetrics>
#include <QString>
#include <QColor>
#include "index.h"
#include "decoratedmove.h"
#include "kgamecanvas.h"

namespace MoveList {

typedef boost::shared_ptr<class Entry> EntryPtr;
typedef boost::shared_ptr<class Brace> BracePtr;
typedef boost::shared_ptr<class Text> TextPtr;
typedef boost::shared_ptr<class Comment> CommentPtr;
typedef std::vector<EntryPtr> History;
typedef std::map<int, History> Variations;
typedef std::map<int, BracePtr> Braces;
typedef std::map<int, CommentPtr> VComments;

class Settings {
public:
  bool anim_enabled;
  bool anim_moving;
  bool anim_hideshow;
  bool anim_highlight;
  int  anim_speed;
  int  anim_smoothness;
  double anim_time;
  QColor select_color;
  QColor comment_color;
  QString icons;

  bool  use_mv_font;
  bool  use_comm_font;
  QFont        mv_font;
  QFontMetrics mv_fmetrics;
  QFont        sel_mv_font;
  QFontMetrics sel_mv_fmetrics;
  QFont        comm_font;
  QFontMetrics comm_fmetrics;

  Settings()
    : mv_fmetrics(QFont())
    , sel_mv_fmetrics(QFont())
    , comm_fmetrics(QFont()) {}

  void load();
  void save();
};

class FancyItem : public KGameCanvasItem {
public:
  int time_pos;
  QPoint target_pos;
  QPoint old_pos;

  int time_opacity;
  int target_opacity;
  int old_opacity;

  bool highlighted;
  int curr_highlight;
  int time_highlight;
  int target_highlight;
  int old_highlight;

  FancyItem(KGameCanvasAbstract* c = NULL)
    : KGameCanvasItem(c)
    , time_pos(-1)
    , time_opacity(-1)
    , highlighted(false)
    , curr_highlight(0)
    , time_highlight(-1) { }
  bool showing();
  void appear();
  void disappear();
  void goTo(QPoint pos);
  void setHighlight(bool h);
  virtual bool canStop() { return time_highlight==-1 && time_pos==-1 && time_opacity==-1; }
  virtual void advance(int time);
  virtual bool layered() const;
};

class Brace : public FancyItem {
public:
  int  width;
  int  height;
  int  depth;
  Entry* entry;
  int variation;
  int time_height;
  int target_height;
  int old_height;

  virtual void  paint (QPainter *p);
  virtual QRect rect () const;
  void setHeight(int h);
  virtual bool canStop() { return time_height==-1 && FancyItem::canStop(); }
  virtual void advance(int time);

  Brace(Entry* e, int var, KGameCanvasAbstract* c = NULL)
    : FancyItem(c)
    , width(0)
    , height(0)
    , entry(e)
    , variation(var)
    , time_height(-1) {
  }
};

class Text : public FancyItem {
public:
  int  width;
  int  height;
  int  type;
  bool selected;
  QString text;
  Entry *entry;
  bool needs_update;

  void doUpdate();
  virtual void  paint (QPainter *p);
  virtual QRect rect () const;

  Text(Entry* e, int t, KGameCanvasAbstract* c = NULL)
    : FancyItem(c)
    , width(0)
    , height(0)
    , type(t)
    , selected(false)
    , entry(e)
    , needs_update(true) {}
};

class Comment : public FancyItem {
public:
  int  width;
  int  height;
  QString text;
  Entry *entry;
  int variation;
  bool needs_update;

  void doUpdate();
  virtual void  paint (QPainter *p);
  virtual QRect rect () const;

  Comment(Entry* e, KGameCanvasAbstract* c = NULL, int v = -1)
    : FancyItem(c)
    , width(0)
    , height(0)
    , entry(e)
    , variation(v)
    , needs_update(true) {}
};

class Entry : public FancyItem {
public:
  bool expanded;
  bool hide_next;
  bool selected;
  QRect m_rect;
  int  m_ascent;
  int  childs_height;
  int  move_turn;
  DecoratedMove move;
  Index index;
  bool needs_update;

  TextPtr number;
  TextPtr fregna;
  CommentPtr comment;

  Variations variations;
  Braces braces;
  VComments vcomments;

  void doUpdate();
  virtual void  paint (QPainter *p);
  virtual QRect rect () const;
  virtual bool canStop() { return time_highlight==-1 && FancyItem::canStop(); }

  Entry(int turn, const DecoratedMove& m, const Index& i, KGameCanvasAbstract* c = NULL)
    : FancyItem(c)
    , expanded(true)
    , hide_next(false)
    , selected(false)
    , move_turn(turn)
    , move(m)
    , index(i)
    , needs_update(true) {}
};

}

#endif //MOVELIST_P_H
