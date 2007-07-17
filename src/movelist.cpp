/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QApplication>
#include <QPainter>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QTextEdit>
#include <QMenu>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QScrollBar>
#include <QTimer>
#include <cmath>
#include <iostream>
#include <kstandarddirs.h>
#include "global.h"
#include "pref_theme.h"
#include "movelist_widget.h"
#include "movelist_table.h"
#include "movelist_notifier.h"
#include "movelist_p.h"

namespace MoveList {

#define MARGIN_LEFT    2
#define MARGIN_RIGHT   4
#define MARGIN_TOP     1
#define MARGIN_BOTTOM  0
#define MIDDLE_PAD          3
#define COMMENT_INDENTATION 4
#define VAR_INDENTATION    16
#define BORDER_LEFT   3
#define BORDER_RIGHT  3
#define BORDER_TOP    3
#define BORDER_BOTTOM 3
#define MIN_COL_WIDTH 5.0

#define DEFAULT_ANIMATION_TIME 350.0

//BEGIN FancyItem--------------------------------------------------------------

bool FancyItem::showing() {
  return !(time_opacity!=-1 && target_opacity == 0) &&
    ((time_opacity!=-1 && target_opacity == 255) || (visible() && opacity() == 255));
}

void FancyItem::appear() {
  if((time_opacity!=-1 && target_opacity == 255)
                || (visible() && opacity() == 255))
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(!m->m_settings->anim_enabled || !m->m_settings->anim_hideshow)
    show();
  else {
    old_opacity = 0;
    target_opacity = 255;
    time_opacity = m->layout_time;
    setAnimated(true);
  }
}

void FancyItem::disappear() {
  if((time_opacity!=-1 && target_opacity == 0)
                  || !visible() || opacity() == 0)
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  setHighlight(false);
  if(!m->m_settings->anim_enabled || !m->m_settings->anim_hideshow)
    hide();
  else {
    old_opacity = 255;
    target_opacity = 0;
    time_opacity = m->layout_time;
    setAnimated(true);
  }
}

void FancyItem::goTo(QPoint p) {
  if((time_pos!=-1 && target_pos == p) || (time_pos==-1 && pos() == p))
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(!m->m_settings->anim_enabled || !m->m_settings->anim_moving)
    moveTo(p);
  else {
    old_pos = pos();
    target_pos = p;
    time_pos = m->layout_time;
    /*std::cout << m->layout_time << " start " << this << " "
            << prettyTypeName(typeid(*this).name()) << std::endl;*/
    setAnimated(true);
  }
}

void FancyItem::setHighlight(bool h) {
  if(highlighted == h)
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(!m->m_settings->anim_enabled || !m->m_settings->anim_highlight) {
    curr_highlight = h ? 255 : 0;
    highlighted = h;
    changed();
  }
  else {
    old_highlight = highlighted ? 255 : 0;
    target_highlight = h ? 255 : 0;
    highlighted = h;
    time_highlight = m->mSecs();
    setAnimated(true);
  }
}

void FancyItem::advance(int time) {
  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  /*std::cout << time << " anim " << this << " "
            << prettyTypeName(typeid(*this).name()) << std::endl;*/
  if(time_highlight != -1) {
    float fact = (time - time_highlight) / m->m_settings->anim_time;
    if(fact >= 1.0) {
      curr_highlight = target_highlight;
      time_highlight = -1;
    }
    else
      curr_highlight = int(target_highlight*fact + old_highlight*(1-fact));
    changed();
  }
  if(time_opacity != -1) {
    float fact = (time - time_opacity) / m->m_settings->anim_time;
    if(fact >= 1.0) {
      setOpacity(target_opacity);
      setVisible(target_opacity != 0);
      time_opacity = -1;
    }
    else {
      setOpacity( int(target_opacity*fact + old_opacity*(1-fact)) );
      setVisible(true);
    }
  }
  if(time_pos != -1) {
    float fact = (time - time_pos) / m->m_settings->anim_time;
    if(fact >= 1.0) {
      /*std::cout << time << " done " << this << " "
            << prettyTypeName(typeid(*this).name()) << std::endl;*/
      moveTo(target_pos);
      time_pos = -1;
    }
    else {
      /*std::cout << time << " move " << this << " "
            << prettyTypeName(typeid(*this).name()) << std::endl;*/
      moveTo( int(target_pos.x()*fact + old_pos.x()*(1-fact)+0.5),
             int(target_pos.y()*fact + old_pos.y()*(1-fact)+0.5));
    }
  }
  if(canStop()) {
    /*std::cout << time << " stop " << this << " "
            << prettyTypeName(typeid(*this).name()) << std::endl;*/
    setAnimated(false);
  }
}

bool FancyItem::layered() const {
  return false;
}

//END FancyItem----------------------------------------------------------------


//BEGIN Brace------------------------------------------------------------------

void Brace::setHeight(int h) {
  if(h<0) h=0;
  if((animated() && target_height == h) || height == h)
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(!m->m_settings->anim_enabled || !m->m_settings->anim_moving) {
    height = h;
    changed();
  }
  else {
    old_height = height;
    target_height = h;
    time_height = m->layout_time;
    if(visible())
      setAnimated(true);
  }
}

void Brace::advance(int time) {
  if(time_height != -1) {
    Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
    float fact = (time - time_height) / m->m_settings->anim_time;
    if(fact >= 1.0) {
      height = target_height;
      time_height = -1;
    }
    else
      height = int(target_height*fact + old_height*(1-fact) + 0.5);
    changed();
  }
  FancyItem::advance(time);
}

void Brace::paint (QPainter *p) {
  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(height < m->entry_size)
    return;
  QPointF p1((pos().x()*2+width)/2.0, (pos().y()*2+m->entry_size)/2.0);
  QPointF p2((pos().x()*2+width)/2.0, (pos().y()*2+height*2-m->entry_size)/2.0);
  p->save();
  QPen q;
  if(depth % 2) {
    p->setBrush(QColor(255,192,224));
    q = QColor(128,0,64);
  }
  else {
    p->setBrush(QColor(192,255,224));
    q = QColor(0,128,64);
  }
  q.setWidth(2);
  p->setPen(q);
  p->setRenderHint(QPainter::Antialiasing);
  int s = std::min(m->entry_size, width);
  float cs1 = (0.4 + 0.2*(curr_highlight/255.0)) * s;
  float cs2 = 0.2 * s;

  p->drawLine(p1,p2);
  p->drawEllipse(QRectF(-cs1/2,-cs1/2,cs1,cs1).translated(p1));
  p->setBrush(p->pen().color());
  p->drawEllipse(QRectF(-cs2/2,-cs2/2,cs2,cs2).translated(p2));
  p->restore();
}

QRect Brace::rect () const {
  return QRect(pos(), QSize(width, height));
}

//END Brace--------------------------------------------------------------------


//BEGIN Text-------------------------------------------------------------------

void Text::paint (QPainter *p) {
  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(curr_highlight != 0) {
    p->setBrush(QColor(192,224,208, curr_highlight));
    p->setPen(QColor(64,128,96, curr_highlight));
    p->drawRect(rect().adjusted(0,0,-1,-1));
  }
  p->setFont(selected ? m->m_settings->sel_mv_font : m->m_settings->mv_font);
  p->setPen(selected ? m->m_settings->select_color : Qt::black);
  p->drawText(pos()+QPoint(MARGIN_LEFT, MARGIN_TOP+m->m_settings->mv_fmetrics.ascent()), text);
}

QRect Text::rect () const {
  return QRect(pos(), QSize(width, height));
}

void Text::doUpdate () {
  if(!needs_update)
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  width = (selected ? m->m_settings->sel_mv_fmetrics
                  : m->m_settings->mv_fmetrics).boundingRect(text).right()
                                      + MARGIN_LEFT + MARGIN_RIGHT;
  height = m->entry_size;

  needs_update = false;
  changed();
}

//END Text---------------------------------------------------------------------


//BEGIN Comment----------------------------------------------------------------

void Comment::paint (QPainter *p) {
  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());

  if(curr_highlight != 0) {
    p->setBrush(QColor(255,255,255, curr_highlight));
    p->setPen(QColor(192,192,192, curr_highlight));
    p->drawRect(rect().adjusted(0,0,-1,-1));
  }
  p->setFont(m->m_settings->comm_font);
  p->setPen(m->m_settings->comment_color);
  p->drawText(pos().x() + MARGIN_RIGHT,
              pos().y(),
              width - MARGIN_LEFT - MARGIN_RIGHT, 9999,
              Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, text);
}

QRect Comment::rect () const {
  return QRect(pos(), QSize(width, height));
}

void Comment::doUpdate () {
  if(!needs_update)
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  QPoint dest = ((time_pos != -1) ? target_pos : pos());
  width = std::max(m->width() - dest.x(), m->entry_size);
  height = m->m_settings->comm_fmetrics.boundingRect(0,0,width - MARGIN_LEFT - MARGIN_RIGHT, 99999,
              Qt::AlignLeft|Qt::AlignTop|Qt::TextWordWrap, text).height()
              + MARGIN_TOP + MARGIN_BOTTOM;

  needs_update = false;
  changed();
}

//END Comment------------------------------------------------------------------


//BEGIN Entry------------------------------------------------------------------

void Entry::paint (QPainter *p) {
  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  if(curr_highlight != 0) {
    p->setBrush(QColor(192,224,255, curr_highlight));
    p->setPen(QColor(64,96,128, curr_highlight));
    p->drawRect(rect().adjusted(0,0,-1,-1));
  }
  p->setPen(selected ? m->m_settings->select_color : Qt::black);
  int x = pos().x()+MARGIN_LEFT;
  int y = pos().y()+MARGIN_TOP+m_ascent;

  p->setRenderHint(QPainter::TextAntialiasing);
  QFont tf = selected ? m->m_settings->sel_mv_font : m->m_settings->mv_font;
  QFontMetrics& fm = selected ? m->m_settings->sel_mv_fmetrics : m->m_settings->mv_fmetrics;
  QRect r(0,0,0,0);

  for(int i=0;i<(int)move.size();i++) {
    if(move[i].m_type == MovePart::Text) {
      p->setFont(tf);
      p->drawText(QPoint(x+r.width(), y), move[i].m_string);
      QRect b = fm.boundingRect(move[i].m_string);
      r |= b.translated(r.width()-b.x(), 0);
    }
    else if(move[i].m_type == MovePart::Figurine) {
      ::Loader::Glyph g = m->m_loader.getValue< ::Loader::Glyph>(move[i].m_string);
      p->setFont(g.fontValid() ? g.font() : tf);
      p->drawText(QPoint(x+r.width(), y), g.ch());
      QFontMetrics fi(g.fontValid() ? g.font() : tf);
      QRect b = fi.boundingRect(g.ch());
      r |= b.translated(r.width()-b.x(), 0);
    }
  }
}

QRect Entry::rect () const {
  return m_rect.translated(pos().x()+MARGIN_LEFT, pos().y()+MARGIN_TOP+m_ascent);
}

void Entry::doUpdate () {
  if(!needs_update)
    return;

  Widget *m = dynamic_cast<Widget*>(topLevelCanvas());
  QFont tf = selected ? m->m_settings->sel_mv_font : m->m_settings->mv_font;
  QFontMetrics& fm = selected ? m->m_settings->sel_mv_fmetrics : m->m_settings->mv_fmetrics;
  m_ascent = m->m_settings->mv_fmetrics.ascent();
  m_rect = QRect(0,0,0,0);

  for(int i=0;i<(int)move.size();i++) {
    if(move[i].m_type == MovePart::Text) {
      QRect b = fm.boundingRect(move[i].m_string);
      m_rect |= b.translated(m_rect.width()-b.x(), 0);
    }
    else if(move[i].m_type == MovePart::Figurine) {
      ::Loader::Glyph g = m->m_loader.getValue< ::Loader::Glyph>(move[i].m_string);
      QFontMetrics fi(g.fontValid() ? g.font() : tf);
      QRect b = fi.boundingRect(g.ch());
      m_rect |= b.translated(m_rect.width()-b.x(), 0);
    }
  }
  m_rect = QRect(m_rect.x(),m_rect.y(),m_rect.width()+MARGIN_RIGHT,m_rect.height());

  needs_update = false;
  changed();
}

//END Entry--------------------------------------------------------------------


//BEGIN Settings---------------------------------------------------------------

void Settings::load() {
  ::Settings s = settings.group("move-list");
  ::Settings s_anim = s.group("animations");

  anim_enabled = s.group("animations").flag("enabled", true);
  anim_moving = s_anim.group("moving").flag("enabled", true);
  anim_hideshow = s_anim.group("hideshow").flag("enabled", true);
  anim_highlight = s_anim.group("highlight").flag("enabled", true);
  anim_speed = s_anim["speed"] | 16;
  anim_time = DEFAULT_ANIMATION_TIME*pow(5.0, 1.0 - anim_speed/16.0);
  anim_smoothness = s_anim["smoothness"] | 16;
  select_color =  s["select-color"] | QColor(Qt::red);
  comment_color = s["comment-color"] | QColor(64,64,64);
  mv_font = QApplication::font();
  if ((use_mv_font = s.group("moves-font").flag("enabled", true)))
    mv_font = s["moves-font"] | mv_font;
  sel_mv_font = mv_font;
  sel_mv_font.setBold(true);
  comm_font = QApplication::font();
  comm_font.setItalic(true);
  if ((use_comm_font = s.group("comment-font").flag("enabled", true)))
    comm_font = s["CommentFont"] | comm_font;
  mv_fmetrics = QFontMetrics(mv_font);
  sel_mv_fmetrics = QFontMetrics(sel_mv_font);
  comm_fmetrics = QFontMetrics(comm_font);

//  settings.qSettings()->endGroup();
}

void Settings::save() {
  ::Settings s = settings.group("move-list");
  ::Settings s_anim = s.group("animations");

  s.group("animations").setFlag("enabled", anim_enabled);
  s_anim.group("moving").setFlag("enabled", anim_moving);
  s_anim.group("hideshow").setFlag("enabled", anim_hideshow);
  s_anim.group("highlight").setFlag("enabled", anim_highlight);
  s_anim["speed"]     = anim_speed;
  s_anim["smoothness"] = anim_smoothness;
  s["select-color"]      = select_color;
  s["comment-color"]     = comment_color;
  s.group("moves-font").flag("enabled", use_mv_font);
  s["moves-font"]       = mv_font;
  s.group("comment-font").flag("enabled", use_comm_font);
  s["comment-font"]       = comm_font;
}

//END Settings-----------------------------------------------------------------


//BEGIN Widget-----------------------------------------------------------------

Widget::Widget(QWidget *parent, Table *o)
: KGameCanvasWidget(parent)
, curr_highlight(-1)
, curr_selected(-1)
, comment_editor(NULL)
, layout_pending(false)
, layout_style(0)
, layout_goto_selected(false)
, layout_width_changed(true)
, layout_must_relayout(true)
, notifier(NULL)
, owner_table(o)
, m_settings(new Settings) {

  resize(50,100);
  setSizePolicy ( QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );

  setMouseTracking(true);
  settingsChanged();
  reset();
}

Widget::~Widget() {
  delete m_settings;
}

void Widget::reset() {
  curr_highlight = Index(-1);
  curr_selected = Index(-1);
  if(comment_editor) {
    delete comment_editor;
    comment_editor = NULL;
  }

  DecoratedMove mv;
  mv.push_back(MovePart(QString("Mainline:")));
  history.clear();
  history.push_back( EntryPtr(new Entry(-1, mv, Index(0), this)) );

  layout();
}

EntryPtr Widget::fetch(const Index& ix) {
  int at;
  History *vec = fetchRef(ix, &at);
  return vec ? (*vec)[at] : EntryPtr();
}

History* Widget::fetchRef(const Index& ix, int* idx) {
  if(ix.num_moves >= (int)history.size() || ix.num_moves < 0 )
    return NULL;

  History* aretv = &history;
  EntryPtr retv = history[ix.num_moves];
  if(idx) *idx = ix.num_moves;

  for(int i=0; i<(int)ix.nested.size();i++) {
    Variations::iterator it = retv->variations.find(ix.nested[i].variation);
    if(it == retv->variations.end() || ix.nested[i].num_moves >= (int)it->second.size()
                                || ix.nested[i].num_moves < 0 )
      return NULL;

    aretv = &it->second;
    retv = it->second[ix.nested[i].num_moves];
    if(idx) *idx = ix.nested[i].num_moves;
  }
  return aretv;
}

Notifier* Widget::getNotifier() {
  return notifier;
}

void Widget::setNotifier(Notifier* n, bool detach_prev){
  if(detach_prev && notifier && notifier != n)
    notifier->onDetachNotifier();
  notifier = n;
}

void Widget::settingsChanged() {
  m_settings->load();

  setAnimationDelay( int(70.0*pow(10.0, -m_settings->anim_smoothness/32.0)) );

  entry_size = m_settings->mv_fmetrics.height()+MARGIN_TOP+MARGIN_BOTTOM;
  owner_table->m_scroll_area->setMinimumSize(entry_size*6, entry_size*9);

  m_loader.setSize(m_settings->mv_font.pointSize());

  layout_must_relayout = true;
  layout();
}

void Widget::mouseMoveEvent ( QMouseEvent * event ) {
  KGameCanvasItem *i = itemAt(event->pos());
  Entry* e = i ? dynamic_cast<Entry*>(i) : NULL;
  Text* f = i ? dynamic_cast<Text*>(i) : NULL;
  Brace* b = i ? dynamic_cast<Brace*>(i) : NULL;
  Comment* c = i ? dynamic_cast<Comment*>(i) : NULL;
  EntryPtr olde = fetch(curr_highlight);
  f = f && f->type == 1 ? f : NULL;

  int oldtype = curr_highlight_type;

  if(e) {
    if(curr_highlight == e->index && curr_highlight_type==-1)
      return;
    e->setHighlight(true);
    curr_highlight = e->index;
    curr_highlight_type = -1;
  }
  else if(f) {
    if(curr_highlight == f->entry->index && curr_highlight_type==-2)
      return;
    f->setHighlight(true);
    curr_highlight = f->entry->index;
    curr_highlight_type = -2;
  }
  else if(c) {
    if( (curr_highlight==c->entry->index) &&
        ((c->variation==-1) ? (curr_highlight_type == -3) :
          (curr_highlight_type == -1000-c->variation) ) )
      return;
    c->setHighlight(true);
    curr_highlight = c->entry->index;
    curr_highlight_type = (c->variation==-1) ? -3 : (-1000-c->variation);
  }
  else if(b) {
    if(curr_highlight == b->entry->index && curr_highlight_type==b->variation)
      return;
    b->setHighlight(true);
    curr_highlight = b->entry->index;
    curr_highlight_type = b->variation;
  }
  else
    curr_highlight = Index(-1);

  if(olde) {
    if(oldtype==-1)
      olde->setHighlight(false);
    else if(oldtype==-2 && olde->fregna)
      olde->fregna->setHighlight(false);
    else if(oldtype==-3 && olde->comment)
      olde->comment->setHighlight(false);
    else if(oldtype<=-1000 && olde->vcomments.count(-1000-oldtype)==1)
      olde->vcomments[-1000-oldtype]->setHighlight(false);
    else if(oldtype>=0 && olde->braces.count(oldtype)==1)
      olde->braces[oldtype]->setHighlight(false);
  }
}

void Widget::startEditing(const Index& i, int v) {
  stopEditing();

  EntryPtr e = fetch(i);
  if(!e) {
    std::cout << "--> Error in Widget::startEditing! Invalid index!" << std::endl;
    return;
  }

  CommentPtr c = v == -1 ? e->comment : (e->vcomments.count(v) ?
                                e->vcomments[v] : CommentPtr());
  QRect rect;
  if(c)
    rect = c->rect();
  else {
    TextPtr n = e->number;
    if(!n && i > Index(0))
      n = fetch(i.prev())->number;
    int x = (n ? n->pos().x() : e->pos().x()) + ((v == -1) ? 0 : VAR_INDENTATION);

    rect = QRect(x, e->pos().y()+entry_size, width()-x, 0);
  }

  edited_comment_variation = v;
  edited_comment = boost::weak_ptr<Entry>(e);
  comment_editor = new QTextEdit(c ? c->text : QString(), this);
  comment_editor->setGeometry(rect.adjusted(0,0,0,entry_size*3));
  comment_editor->show();
  comment_editor->setFocus(Qt::MouseFocusReason);
  comment_editor->installEventFilter(this);
}

bool Widget::eventFilter(QObject *obj, QEvent *event) {
  if(obj == comment_editor && event->type() == QEvent::FocusOut ) {
    stopEditing();
    return true;
  }
  return false;
}

void Widget::stopEditing() {
  EntryPtr e = edited_comment.lock();
  if(e) {
    if(comment_editor && notifier) {
      QString c = comment_editor->toPlainText();
      c.replace(QRegExp("(?:[ \t]\r?\n\r?|\r?\n\r?[ \t]|\r?\n\r?)"), " ");
      if(edited_comment_variation == -1)
        notifier->onUserSetComment(e->index, c);
      else
        notifier->onUserSetVComment(e->index, edited_comment_variation, c);
    }
    edited_comment.reset();
  }
  if(comment_editor) {
    comment_editor->deleteLater();
    comment_editor = NULL;
  }
}

void Widget::mousePressEvent ( QMouseEvent * event ) {
  stopEditing();

  KGameCanvasItem *i = itemAt(event->pos());
  if(!i)
    return;

  Text *t = dynamic_cast<Text*>(i);
  if(t && t->type == 1) {
    Entry *e = t->entry;
    if(e->hide_next) {
      e->hide_next = false;
      e->expanded = true;
    }
    else
      e->expanded = !e->expanded;
    layout();
    return;
  }

  Brace *b = dynamic_cast<Brace*>(i);
  if(b) {
    if(event->button() == Qt::LeftButton) {
      Entry* e = b->entry;
      EntryPtr first = e->variations[b->variation][0];
      first->hide_next = !first->hide_next;
      layout();
    }
    else if(event->button() == Qt::RightButton) {
      QAction *a;
      QMenu m(this);
      a = m.addAction(QIcon(m_settings->icons+"comment.png"), "&Set comment");
      a->setData("comment");
      m.addSeparator();
      a = m.addAction(QIcon(m_settings->icons+"promote.png"), "&Promote variation");
      a->setData("promote");
      a = m.addAction(QIcon(m_settings->icons+"remove.png"), "&Remove variation");
      a->setData("remove");
      boost::weak_ptr<Entry> ewptr = boost::weak_ptr<Entry>(fetch(b->entry->index));
      int v = b->variation;

      a = m.exec(event->globalPos());

      /* beware, here, after exec, e could be a dangling pointer */
      EntryPtr eptr = ewptr.lock();
      if(a && notifier && eptr && eptr->variations.count(v)) {
        if(a->data() == "comment")
          startEditing(eptr->index, v);
        else if(a->data() == "promote")
          notifier->onUserPromoteVariation(eptr->index.next(v));
        else if(a->data() == "remove")
          notifier->onUserRemoveVariation(eptr->index.next(v));
      }
    }
    return;
  }

  Comment *c = dynamic_cast<Comment*>(i);
  if(c) {
    startEditing(c->entry->index, c->variation);
    return;
  }

  Entry *e = dynamic_cast<Entry*>(i);
  if(e) {
    if(event->button() == Qt::LeftButton) {
      if(notifier)
        notifier->onUserSelectMove(e->index);
    }
    else if(event->button() == Qt::RightButton) {
      QAction *a;
      QMenu m(this);
      a = m.addAction(QIcon(m_settings->icons+"comment.png"), "&Set comment");
      a->setData("comment");
      a = m.addAction(QIcon(m_settings->icons+"clear.png"), "&Clear variations");
      a->setEnabled(!e->variations.empty());
      a->setData("clear");
      a = m.addAction(QIcon(m_settings->icons+"truncate.png"), "&Truncate");
      a->setEnabled(fetch(e->index.next()));
      a->setData("truncate");
      m.addSeparator();
      a = m.addAction(QIcon(m_settings->icons+"promote.png"), "&Promote variation");
      a->setEnabled(e->index.nested.size());
      a->setData("promote");
      a = m.addAction(QIcon(m_settings->icons+"remove.png"), "&Remove variation");
      a->setEnabled(e->index.nested.size());
      a->setData("remove");
      boost::weak_ptr<Entry> ewptr = boost::weak_ptr<Entry>(fetch(e->index));

      a = m.exec(event->globalPos());

      /* beware, here, after exec, e could be a dangling pointer */
      EntryPtr eptr = ewptr.lock();
      if(a && notifier && eptr) {
        if(a->data() == "comment")
          startEditing(eptr->index, -1);
        else if(a->data() == "clear")
          notifier->onUserClearVariations(eptr->index);
        else if(a->data() == "truncate")
          notifier->onUserTruncate(eptr->index);
        else if(a->data() == "promote")
          notifier->onUserPromoteVariation(eptr->index);
        else if(a->data() == "remove")
          notifier->onUserRemoveVariation(eptr->index);
      }
    }
    return;
  }
}

void Widget::mouseReleaseEvent ( QMouseEvent * /*event*/ ) {

}

void Widget::resizeEvent ( QResizeEvent * event ) {
  stopEditing();
  if(event->size().width() != event->oldSize().width()) {
    layout_width_changed = true;
    layout();
  }
}

void Widget::layout() {
  if(layout_pending)
    return;

  layout_pending = true;
  QTimer::singleShot( 0, this, SLOT(doLayout()) );
}

void Widget::doLayout() {
  layout_time = mSecs();
  layout_pending = false;
  layout_max_width = 0;
  //std::cout << "layout_must_relayout = " << layout_must_relayout << std::endl;
  int h = layoutHistory(history, BORDER_LEFT, BORDER_TOP, -1, 0, 0, true);

  QSize s(std::max(entry_size*7, layout_max_width+BORDER_RIGHT),
                                     std::max(entry_size*10, h+BORDER_BOTTOM) );
  setMinimumSize(s);

  layout_width_changed = false;
  layout_must_relayout = false;
  if(layout_goto_selected) {
    EntryPtr e = fetch(curr_selected);
    if(e)
      owner_table->m_scroll_area->ensureVisible( int(e->pos().x() + e->m_rect.width()*0.5),
                                                 int(e->pos().y() + e->m_rect.height()*0.5) );
    layout_goto_selected = false;
  }
}

int Widget::layoutHistory(History& array, int at_x, int at_y,
                      int a_prev_turn, int mv_num, int sub_mv_num, bool visible) {
  int flow_y = at_y;
  int flow_x = at_x;
  int nflow_x = at_x;
  int col_num = 0;
  int prev_turn = a_prev_turn;

  for(int i=0;i<(int)array.size();i++) {
    EntryPtr e = array[i];

    /* if this is not visible, hide the item and hide all the number/fregna tags */
    if(!visible) {
      e->disappear();
      if(e->number)
        e->number->disappear();
      if(e->fregna)
        e->fregna->disappear();
      if(e->comment)
        e->comment->disappear();

      /* hide the subvariations */
      for(Variations::iterator it = e->variations.begin(); it != e->variations.end(); ++it)
        layoutHistory(it->second, 0, 0, e->move_turn, mv_num, sub_mv_num, false);
      for(Braces::iterator it = e->braces.begin(); it != e->braces.end(); ++it)
        it->second->disappear();
      for(VComments::iterator it = e->vcomments.begin(); it != e->vcomments.end(); ++it)
        it->second->disappear();
      mv_num++;
      continue;
    }

    /* adjust the position if this is paired on the right  */
    bool draw_num = false;

    {
      if(e->move_turn != prev_turn) {
        mv_num++;
        sub_mv_num = 0;
      }
      else
        sub_mv_num++;

      if(layout_style==0) {
        if(e->move_turn == 0 || i==0 || array[i-1]->childs_height != 0) {
          if(mv_num>=1 && (e->move_turn != prev_turn || i==0 || array[i-1]->childs_height != 0)) {
            draw_num = true;
            flow_x = at_x;
          }
          else
            flow_x = (mv_num>=1) ? nflow_x : at_x;
        }
        else {
          if(e->move_turn != prev_turn) {
            flow_x = std::max(flow_x + MIDDLE_PAD, int(MIN_COL_WIDTH*entry_size));
            flow_y -= entry_size;
          }
          else
            flow_x = int(MIN_COL_WIDTH*entry_size);
        }
      }
      else {
        if(e->move_turn != prev_turn || i==0 || array[i-1]->childs_height != 0) {
          col_num = 0;
          flow_x = at_x;
          if(mv_num>=1)
            draw_num = true;
        }
        else if(col_num == layout_style) {
          col_num = 0;
          flow_x = nflow_x;
        }
        else {
          flow_y -= entry_size;
          flow_x = std::max(flow_x + MIDDLE_PAD,
                              at_x + col_num*int(MIN_COL_WIDTH*entry_size));
        }
      }
    }

    col_num++;

    /* update the number */
    if(draw_num) {
      TextPtr& n = e->number;
      if(!n) {
        n = TextPtr(new Text(e.get(), 0, this));
        if(layout_style==0)
          n->text = QString::number((mv_num+1)/2)+(mv_num&1 ? "." : ". ...");
        else
          n->text = QString::number(mv_num)+
                  (sub_mv_num ? "+"+QString::number(sub_mv_num) : QString())+".";
        n->needs_update = true;
      }
      else if( !n->showing() || layout_must_relayout)
        n->needs_update = true;

      /* Mh, the number should never change, only appear disappear.
         should this change, add here the code to enable number changes. */
      QPoint dest(flow_x, flow_y);

      if(n->pos() != dest)
        if(n->visible())
          n->goTo(dest);
        else
          n->moveTo(dest);
      n->doUpdate();
      n->appear();

      flow_x += n->width;
      nflow_x = flow_x;
    }
    else if(e->number)
      e->number->disappear();


    /* update the entry */
    QPoint dest(flow_x, flow_y);
    if(e->pos() != dest)
      if(e->visible())
        e->goTo(dest);
      else
        e->moveTo(dest);
    if( !e->showing() || layout_must_relayout)
      e->needs_update = true;
    e->doUpdate();
    e->appear();
    e->childs_height = 0;
    flow_x += e->m_rect.width();


    /* Update the fregna. The fregna is visible if there are subvariations in this
       entry, or if this entry is the first one of a variation where the remaining
       entries are hidden and that contains the current position */
    bool expandable = !e->variations.empty() || e->comment;
    bool sel = (e->hide_next && e->index<curr_selected)
             || (!e->expanded && expandable &&
                e->index<curr_selected && !(e->index.next()<=curr_selected));
    if(expandable || sel ) {
      if(!e->fregna)
        e->fregna = TextPtr(new Text(e.get(), 1, this));

      /* update the data, if needed */
      TextPtr f = e->fregna;
      const char *text = (sel||!e->expanded||e->hide_next) ? "[+]" : "[-]";
      if(f->text != text || f->selected != sel) {
        f->text = text;
        f->selected = sel;
        f->needs_update = true;
      }
      else if( !f->showing() || layout_must_relayout)
        f->needs_update = true;

      QPoint dest(flow_x, flow_y);

      if(f->pos() != dest)
        if(f->visible())
          f->goTo(QPoint(flow_x, flow_y));
        else
          f->moveTo(QPoint(flow_x, flow_y));
      f->doUpdate();
      f->appear();

      flow_x += f->width;
    }
    else if(e->fregna)
      e->fregna->disappear();

    /* update the flow information */
    flow_y += entry_size;
    layout_max_width = std::max(flow_x, layout_max_width);
    int prev_pos = flow_y;

    /* update the comment */
    if(e->comment) {
      CommentPtr c = e->comment;

      if(e->expanded && !e->hide_next) {
        QPoint dest(at_x + COMMENT_INDENTATION, flow_y);

        if(c->pos() != dest ) {
          if(c->visible())
            c->goTo(dest);
          else
            c->moveTo(dest);
          c->needs_update = true;
        }
        else if( !c->showing() || layout_width_changed || layout_must_relayout)
          c->needs_update = true;
        c->doUpdate();
        c->appear();
        flow_y += c->height;
      }
      else
        c->disappear();
    }

    /* update the variations */
    for(Variations::iterator it = e->variations.begin(); it != e->variations.end(); ++it) {
      int old_pos = flow_y;

      /* update the variation's comment */
      if(e->vcomments.count(it->first)) {
        CommentPtr c = e->vcomments[it->first];

        if(e->expanded && !e->hide_next) {
          QPoint dest(at_x + VAR_INDENTATION + COMMENT_INDENTATION, flow_y);

          if( !c->showing() || layout_must_relayout)
            c->needs_update = true;
          if(c->pos() != dest)
            if(c->visible())
              c->goTo(dest);
            else
              c->moveTo(dest);
          c->doUpdate();
          c->appear();
          flow_y += c->height;
        }
        else
          c->disappear();
      }

      /* layout the variation */
      flow_y = layoutHistory(it->second, at_x + VAR_INDENTATION, flow_y,
                           e->move_turn, mv_num, sub_mv_num, e->expanded && !e->hide_next);

      /* update the brace of the variation */
      BracePtr b = e->braces[it->first];
      if(e->expanded && !e->hide_next) {
        b->depth = e->index.nested.size();
        b->setHeight((it->second.size() && it->second[0]->hide_next) ? entry_size : flow_y - old_pos);
        b->width = VAR_INDENTATION;
        if(b->visible())
          b->goTo(QPoint(at_x, old_pos));
        else
          b->moveTo(QPoint(at_x, old_pos));
        b->appear();
      }
      else
        b->disappear();
    }

    e->childs_height = flow_y - prev_pos;

    if(e->hide_next)
      visible = false;
    prev_turn = e->move_turn;
  }
  return flow_y;
}

QPixmap Widget::getPixmap(const QString& s, bool selected) {
  QString k = selected ? s+"_sel":s;
  if(loaded_pixmaps.contains(k))
    return loaded_pixmaps[k];

  QString iconFile = KStandardDirs::locate("appdata", "piece_icons/" + s + ".png");
  QImage img(iconFile);

  if(selected) {
    QPainter p(&img);
    p.setCompositionMode(QPainter::CompositionMode_SourceAtop );
    p.fillRect(0,0,img.width(), img.height(), m_settings->select_color);
  }
  return loaded_pixmaps[k] = QPixmap::fromImage(img.scaled(m_settings->mv_fmetrics.ascent(),
                                                          m_settings->mv_fmetrics.ascent(),
                                              Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

void Widget::setComment(EntryPtr e, int v, const QString& comment) {
  if(comment.isEmpty()) {
    if(v == -1) {
      if(e->comment)
        e->comment = CommentPtr();
    }
    else {
      if(e->vcomments.count(v))
        e->vcomments.erase(v);
    }
  }
  else {
    CommentPtr p;

    if(v == -1) {
      if(!e->comment)
        e->comment = CommentPtr(new Comment(e.get(), this));
      p = e->comment;
    }
    else {
      if(!e->variations.count(v))
        return;

      if(!e->vcomments.count(v))
        p = e->vcomments[v] = CommentPtr(new Comment(e.get(), this, v));
      else
        p = e->vcomments[v];
    }

    if(p->text == comment)
      return;
    p->text = comment;
    p->needs_update = true;
  }

  layout();
}

void Widget::setComment(const Index& index, const QString& comment) {
  EntryPtr e = fetch(index);
  if(!e) {
    std::cout << "--> Error in Widget::setComment! Invalid index " << index << std::endl;
    return;
  }
  setComment(e, -1, comment);
}

void Widget::setVComment(const Index& index, int v, const QString& comment) {
  EntryPtr e = fetch(index);
  if(!e || !e->variations.count(v)) {
    std::cout << "--> Error in Widget::setVComment! Invalid index " << index << std::endl;
    return;
  }
  setComment(e, v, comment);
}

void Widget::setMove(const Index& index,
          int turn, const QString& move, const QString& comment) {
  DecoratedMove mv;
#if 1
  mv.push_back(MovePart(move));
#else
  //TODO: move this code in some other place, it really should not stay here
  QRegExp reg("[KQRBNP]");
  int x = 0;
  while(reg.indexIn(move, x) != -1) {
    if(reg.pos() > x)
      mv.push_back(MovePart(MoveText, move.mid(x, reg.pos()-x)));
    mv.push_back(MovePart(MovePixmap, reg.cap().toLower()));
    x = reg.pos() + reg.matchedLength();
  }
  if(x<move.length())
    mv.push_back(MovePart(MoveText, move.mid(x)));
#endif
  setMove(index, turn, mv, comment);
}

void Widget::setMove(const Index& index,
          int turn, const DecoratedMove& move, const QString& comment) {
  EntryPtr e = fetch(index);
  if(e) {
    e->move_turn = turn;
    e->move = move;
    e->needs_update = true;
    setComment(e, -1, comment);
    layout();
    return;
  }

  int at;
  History *vec = fetchRef(index.prev(), &at);
  if(!vec) {
    std::cout << "--> Error in Widget::setMove! Invalid index " << index << std::endl;
    return;
  }

  if(index.nested.size() && index.nested[index.nested.size()-1].num_moves == 0) {
    History var;
    int v = index.nested[index.nested.size()-1].variation;
    var.push_back(e = EntryPtr( new Entry(turn, move, index, this)) );
    (*vec)[at]->variations[v] = var;
    (*vec)[at]->braces[v] = BracePtr( new Brace( (*vec)[at].get(), v, this) );
  }
  else
    vec->push_back(e = EntryPtr( new Entry(turn, move, index, this)) );

  setComment(e, -1, comment);
  e->hide();
  layout();
}

void Widget::remove(const Index& index) {

  if(index.atVariationStart() ) {
    EntryPtr e = fetch(index.prev());
    if(!e)
      return;

    int v = index.nested[index.nested.size()-1].variation;
    if(!e->variations.count(v))
      return;

    e->variations.erase(v);
    e->braces.erase(v);
    e->vcomments.erase(v);
  }
  else {
    int at;
    History *vec = fetchRef(index, &at);
    if(!vec)
      return;

    while((int)vec->size() > at)
      vec->pop_back();
  }
  layout();
}

void Widget::fixIndices(const Index& ix) {
  int at;
  History *vec = fetchRef(ix, &at);
  if(!vec) {
    std::cout << "--> Error in Widget::fixIndices, invalid index "<<ix << std::endl;
    return;
  }
  Index index = ix;
  for(int i=at;i<(int)vec->size();i++) {
    EntryPtr e = (*vec)[i];
    e->index = index;

    for(Variations::const_iterator it = e->variations.begin();
            it != e->variations.end(); ++it)
      fixIndices(index.next(it->first));
    index = index.next();
  }
}

void Widget::promoteVariation(const Index& ix, int v) {
  int at;
  History *vec = fetchRef(ix, &at);
  if(!vec) {
    std::cout << "--> Error in Widget::promoteVariation, invalid index "<<ix << std::endl;
    return;
  }

  History vold = (*vec)[at]->variations[v];
  History vnew;
  for(int i=at+1; i<(int)vec->size(); i++)
    vnew.push_back((*vec)[i]);
  while((int)vec->size()>at+1)
    vec->pop_back();
  for(int i=0; i<(int)vold.size(); i++)
    vec->push_back(vold[i]);
  (*vec)[at]->variations[v] = vnew;

  Q_ASSERT((int)vec->size()>at+1);
  (*vec)[at+1]->hide_next = false;

  fixIndices(ix.next());
  fixIndices(ix.next(v));

  curr_selected = curr_selected.flipVariation(ix, v);
  curr_highlight = curr_highlight.flipVariation(ix, v);
  layout();
}

void Widget::select(const Index& index) {
  if(curr_selected == index)
    return;
  EntryPtr e = fetch(index);
  EntryPtr olde = fetch(curr_selected);
  if(olde) {
    olde->selected = false;
    olde->needs_update = true;
  }
  if(e) {
    e->selected = true;
    e->needs_update = true;
    layout_goto_selected = true;
  }
  curr_selected = index;
  layout();
}

//END Widget-------------------------------------------------------------------

} //end namespace MoveList
