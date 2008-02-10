/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifdef HAVE_KHTML

#include <boost/shared_ptr.hpp>
#include <iostream>
#include <QFile>
#include <QHBoxLayout>
#include <QTextStream>
#include <kurl.h>
#include <dom/html_document.h>
#include <dom/html_element.h>
#include <dom/dom_element.h>
#include <dom/dom_text.h>
#include <kstandarddirs.h>
#include "mastersettings.h"
#include "decoratedmove.h"
#include "movelist_p.h"
#include "movelist_textual.h"

namespace MoveList {

Textual::Textual(QWidget *parent)
: KHTMLPart(parent, parent, KHTMLPart::BrowserViewGUI)
, m_curr_selected(-1)
, m_layout_style(0) {

  m_loader.setSize(12);

  connect(browserExtension(), SIGNAL(openUrlRequest(const KUrl&,const KParts::URLArgs&)),
                                                                    this, SLOT(onURL(const KUrl&)));
  setJScriptEnabled(false);
  setJavaEnabled(false);
  setMetaRefreshEnabled(false);
  setPluginsEnabled(false);

  QString fpath = KStandardDirs::locate("appdata", "scripts/movelist_textual.html");
  //kDebug() << "HTML file is:"<< fpath;
  QFile file(fpath);
  file.open(QIODevice::ReadOnly);
  QTextStream stream(&file);
  QString html = stream.readAll();
  //kDebug() << "Using HTML:"<<html;

  begin();
  write(html);
  end();
  setMove(0, -1, "Mainline:");
}

static void clear_node(DOM::Node n) {
  if(!n.isNull())
  while(1) {
    DOM::Node f = n.firstChild();
    if(f.isNull())
      break;
    n.removeChild(f);
  }
}

static void clear_from(DOM::Node n) {
  DOM::Node p = n.parentNode();
  while(1) {
    DOM::Node next = n.nextSibling();
    p.removeChild(n);
    if(next.isNull())
      break;
    n = next;
  }
}

void Textual::onURL(const KUrl &url) {
  QString u = url.url();
  if(!u.startsWith("index://")) {
    kDebug() << " --> Error in Textual::onURL, what should i do with url \""<< u <<"\"?";
    return;
  }
  QString p = u.mid(8);
  Index ix = Index::fromString(p);
  kDebug() << "Open Link \"" << p << "\" = \"" << ix << "\"";
  m_notifier->onUserSelectMove(ix);
}

void Textual::setComment(const Index& index, const QString& comment) {
  DOM::HTMLDocument document = htmlDocument();
  DOM::Element this_cm = document.getElementById("cm_"+index);
  if(this_cm.isNull()) {
    kDebug() << " --> Error in Textual::setComment, invalid index!";
    return;
  }

  clear_node(this_cm);
  if(!comment.isEmpty()) {
    this_cm.appendChild(document.createTextNode(comment));
    this_cm.appendChild(document.createTextNode(QString(" ")));
  }
}

void Textual::setVComment(const Index& index, int v, const QString& comment) {
  DOM::HTMLDocument document = htmlDocument();
  DOM::Element this_cm = document.getElementById("vk_"+index.next(v));
  if(this_cm.isNull()) {
    kDebug() << " --> Error in Textual::setVComment, invalid index!";
    return;
  }

  clear_node(this_cm);
  if(!comment.isEmpty()) {
    this_cm.appendChild(document.createTextNode(comment));
    this_cm.appendChild(document.createTextNode(QString(" ")));
  }
}

void Textual::remove(const Index& index) {
  DOM::HTMLDocument document = htmlDocument();
  if(index.atVariationStart()) {
    DOM::Element vc = document.getElementById("vc_"+index);
    if(!vc.isNull()) {
      /* remove a number? */
      DOM::Element comm(vc.previousSibling());
      DOM::Element mvnum(vc.nextSibling());
      if(!mvnum.isNull() && mvnum.getAttribute("id").string().startsWith("nm_") &&
            !comm.isNull() && comm.getAttribute("id").string().startsWith("cm_"))
      {
        DOM::Element move(mvnum.nextSibling());
        int mv_num = move.getAttribute("mvnum").string().toInt();
        int sub_mv_num = move.getAttribute("submvnum").string().toInt();
        if(!(mv_num>0 && (sub_mv_num==0 && (mv_num%2 || m_layout_style))))
          mvnum.parentNode().removeChild(mvnum);
      }
      vc.parentNode().removeChild(vc);
    }
  }
  else {
    DOM::Element rm = document.getElementById("nm_"+index);
    if(rm.isNull())
      rm = document.getElementById("mv_"+index);
    if(!rm.isNull())
      clear_from(rm);
  }
  if(m_curr_selected >= index)
    m_curr_selected = Index(-1);
}

void Textual::select(const Index& index) {
  if(index == m_curr_selected)
    return;
  DOM::HTMLDocument document = htmlDocument();
  DOM::Element currs = document.getElementById("mv_"+m_curr_selected);
  DOM::Element news = document.getElementById("mv_"+index);
  if(!currs.isNull())
    currs.style().removeProperty("background-color");
  if(!news.isNull())
    news.style().setProperty("background-color", "#C0E0FF", "important");
  m_curr_selected = index;
}


void Textual::setMove(const Index& index, int turn, const QString& move,
                                            const QString& comment) {
  DecoratedMove m;
  m << MovePart(move);
  setMove(index, turn, m, comment);
}

// void Textual::setMove(const Index& index, int turn, const DecoratedMove& move,
//                                                           const QString& comment) {
//   setMove(index, turn, move[0].m_string, comment);
// }

void Textual::setMove(const Index& index, int turn, const DecoratedMove& move,
                                                          const QString& comment) {
  //kDebug() << "i= " << index;
  DOM::HTMLDocument document = htmlDocument();
  QString istr = (QString)index;

  DOM::Element this_mv = document.getElementById("mv_"+istr);
  DOM::Element this_cm = document.getElementById("cm_"+istr);
  if(!this_mv.isNull() && !this_cm.isNull()) {
    clear_node(this_mv);
    for(int i=0;i<move.size();i++) {
      DOM::Text t = document.createTextNode(move[i].m_string);
      this_mv.appendChild(t);
    }

    clear_node(this_cm);
    if(!comment.isEmpty()) {
      this_cm.appendChild(document.createTextNode(comment));
      this_cm.appendChild(document.createTextNode(QString(" ")));
    }
    return;
  }

  DOM::Element parent;
  DOM::Element prev_mv;
  DOM::Element prev_cm;

  if(index != Index(0)) {
    prev_cm = document.getElementById("cm_"+index.prev());
    prev_mv = document.getElementById("mv_"+index.prev());
    if(prev_cm.isNull() || prev_mv.isNull()) {
      kDebug() << " --> Error in Textual::setMove, no previous move!";
      return;
    }
  }

  int mv_num = 0;
  int sub_mv_num = 0;
  if(!prev_mv.isNull()) {
    int prev_turn = prev_mv.getAttribute("turn").string().toInt();
    int prev_mv_num = prev_mv.getAttribute("mvnum").string().toInt();
    int prev_sub_mv_num = prev_mv.getAttribute("submvnum").string().toInt();
    if(prev_turn != turn)
      mv_num = prev_mv_num+1;
    else {
      mv_num = prev_mv_num;
      sub_mv_num = prev_sub_mv_num+1;
    }
  }

  if(!index.nested.size()) {
    parent = document.body();
    if(parent.isNull()) {
      kDebug() << "QUEEEEEEEEE!!!!!!!";
      return;
    }
  }
  else if(index.atVariationStart()) {
    QString var_id = "vc_" + istr;
    DOM::Element add_before = prev_cm.nextSibling();

    while(!add_before.isNull()) {

      QString id = add_before.getAttribute("id").string();
      if(id.startsWith("vc_") && id < var_id)
        add_before = add_before.nextSibling();
      else
        break;
    }

    DOM::Element var_el = document.createElement("span");
    var_el.setAttribute("id", var_id);
    var_el.setAttribute("class", "variation");
    var_el.appendChild(document.createTextNode("[ "));


    parent = document.createElement("span");
    parent.setAttribute("id", "vr_" + istr);
    DOM::Element vk_el = document.createElement("span");
    vk_el.setAttribute("id", "vk_" + istr);
    vk_el.setAttribute("class", "comment");
    //vk_el.setContentEditable(true);
    parent.appendChild(vk_el);
    var_el.appendChild(parent);

    var_el.appendChild(document.createTextNode("] "));
    prev_cm.parentNode().insertBefore(var_el, add_before);

    if(!add_before.isNull() && add_before.getAttribute("id").string().startsWith("mv_")) {
      int mv_num = add_before.getAttribute("mvnum").string().toInt();
      int sub_mv_num = add_before.getAttribute("submvnum").string().toInt();
      QString num_str;
      if(m_layout_style == 0) {
        if(mv_num%2)
          num_str = QString("%1. ").arg((mv_num+1)/2);
        else
          num_str = QString("%1. ... ").arg((mv_num+2)/2);
      }
      else {
        if(sub_mv_num==0)
          num_str = QString("%1. ").arg(mv_num);
        else
          num_str = QString("%1+%2. ").arg(mv_num).arg(sub_mv_num);
      }
      DOM::Element num_el = document.createElement("a");
      num_el.setAttribute("class", "mvnum");
      num_el.setAttribute("id", "nm_"+istr);
      num_el.appendChild(document.createTextNode(num_str));
      prev_cm.parentNode().insertBefore(num_el, add_before);
    }
  }
  else {
    Index pi = index.prev(index.nested.back().num_moves);
    parent = document.getElementById("vr_"+pi);
    if(parent.isNull()) {
      kDebug() << " --> Error in Textual::setMove, no variation?!?!?";
      return;
    }
  }

  if(mv_num>0 && ((sub_mv_num==0 && (mv_num%2 || m_layout_style)) || parent.lastChild() != prev_cm) ) {
    QString num_str;
    if(m_layout_style == 0) {
      if(mv_num%2)
        num_str = QString("%1. ").arg((mv_num+1)/2);
      else
        num_str = QString("%1. ... ").arg((mv_num+2)/2);
    }
    else {
      if(sub_mv_num==0)
        num_str = QString("%1. ").arg(mv_num);
      else
        num_str = QString("%1+%2. ").arg(mv_num).arg(sub_mv_num);
    }
    DOM::Element num_el = document.createElement("a");
    num_el.setAttribute("class", "mvnum");
    num_el.setAttribute("id", "nm_"+istr);
    num_el.appendChild(document.createTextNode(num_str));
    parent.appendChild(num_el);
  }

  DOM::Element mv_el = document.createElement("a");
  mv_el.setAttribute("id", "mv_"+istr);
  mv_el.setAttribute("href", "index://"+istr);
  mv_el.setAttribute("class", "move");
  mv_el.setAttribute("turn", QString::number(turn));
  mv_el.setAttribute("mvnum", QString::number(mv_num));
  mv_el.setAttribute("submvnum", QString::number(sub_mv_num));
  for(int i=0;i<move.size();i++) {
    if(move[i].m_type == MovePart::Figurine) {
      ::Loader::Glyph g = m_loader.getValue< ::Loader::Glyph>(move[i].m_string);
      DOM::Element el = document.createElement("span");
#if 1
      kDebug() << "size = " << QString("%1%").arg(g.m_font.pointSize()*100/12);
      el.style().setProperty("font-size", QString("%1%").arg(g.m_font.pointSize()*100/12), "important");
      el.style().setProperty("line-height", QString("%1%").arg(g.m_font.pointSize()*100/12), "important");
#endif
      kDebug() << "familiy = " << g.m_font.family();
      el.style().setProperty("font-weight", "normal", "important");
      el.style().setProperty("font-family", g.m_font.family(), "important");
      DOM::Text t = document.createTextNode(QString(g.m_char));
      el.appendChild(t);
      mv_el.appendChild(el);
    }
    else {
      DOM::Text t = document.createTextNode(move[i].m_string);
      mv_el.appendChild(t);
    }
  }
  parent.appendChild(mv_el);

  parent.appendChild(document.createTextNode(QString(" ")));

  DOM::Element cm_el = document.createElement("span");
  cm_el.setAttribute("id", "cm_"+istr);
  cm_el.setAttribute("class", "comment");
  if(!comment.isEmpty()) {
    cm_el.appendChild(document.createTextNode(comment));
    cm_el.appendChild(document.createTextNode(QString(" ")));
  }
  //cm_el.setContentEditable(true);
  parent.appendChild(cm_el);
}

void Textual::reset() {
  clear_node(htmlDocument().body());
  setMove(0, -1, "Mainline:");
}

Notifier* Textual::getNotifier() {
  return m_notifier;
}

void Textual::setNotifier(Notifier* n, bool detach_prev) {
  if(detach_prev && m_notifier && m_notifier != n)
    m_notifier->onDetachNotifier();
  m_notifier = n;
}

int Textual::layoutStyle() {
  return m_layout_style;
}

void Textual::setLayoutStyle(int x) {
  m_layout_style = x;
}


} //end namespace MoveList

#endif //HAVE_KHTML
