/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QApplication>
#include <QMenu>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QToolButton>
#include <QTabWidget>
#include <QScrollBar>
#include <kicon.h>
#include "mastersettings.h"
#include "movelist_textual.h"
#include "movelist_notifier.h"
#include "movelist_widget.h"
#include "movelist_table.h"

namespace MoveList {

//BEGIN Table------------------------------------------------------------------

Table::Table(QWidget* w)
: QWidget(w) {
  setWindowTitle("Move List");

  QVBoxLayout *vbox = new QVBoxLayout(this);
  QTabWidget *tw = new QTabWidget(this);
  tw->setTabPosition(QTabWidget::West);
  vbox->addWidget(tw);

  m_scroll_area = new QScrollArea(tw);
  m_movelist = new Widget(m_scroll_area, this);
  m_scroll_area->setFocusPolicy(Qt::NoFocus);
  m_scroll_area->setWidgetResizable(true);
  m_scroll_area->setWidget(m_movelist);
  m_scroll_area->resize(50,100);
  tw->addTab(m_scroll_area, KIcon("prefMoveList"), "&List");

  m_movelist_textual = NULL;
  //m_movelist_textual = new Textual(tw);
  //tw->addTab(m_movelist_textual->widget(), KIcon("html"), "&Html");
}

void Table::settingsChanged() {
  if(m_movelist) m_movelist->settingsChanged();
  if(m_movelist_textual) m_movelist_textual->settingsChanged();
}

void Table::setLoaderTheme(const ThemeInfo& theme) {
  if(m_movelist) m_movelist->setLoaderTheme(theme);
  if(m_movelist_textual) m_movelist_textual->setLoaderTheme(theme);
}

void Table::onUndo() {
  if(m_movelist->notifier)
    if(m_movelist) m_movelist->notifier->onUserUndo();
}

void Table::onRedo() {
  if(m_movelist->notifier)
    if(m_movelist) m_movelist->notifier->onUserRedo();
}

void Table::setComment(const Index& index, const QString& comment) {
  if(m_movelist) m_movelist->setComment(index, comment);
  if(m_movelist_textual) m_movelist_textual->setComment(index, comment);
}

void Table::setVComment(const Index& index, int v, const QString& comment, bool confirm_promotion) {
  if(!confirm_promotion)
    if(m_movelist) m_movelist->setVComment(index, v, comment);
  if(m_movelist_textual) m_movelist_textual->setVComment(index, v, comment);
}

void Table::setMove(const Index& index, int turn, const DecoratedMove& move,
                                const QString& comment, bool confirm_promotion) {
  if(!confirm_promotion)
    if(m_movelist) m_movelist->setMove(index, turn, move, comment);
  if(m_movelist_textual) m_movelist_textual->setMove(index, turn, move, comment);
}

void Table::setMove(const Index& index, int turn, const QString& move,
                                const QString& comment, bool confirm_promotion) {
  if(!confirm_promotion)
    if(m_movelist) m_movelist->setMove(index, turn, move, comment);
  if(m_movelist_textual) m_movelist_textual->setMove(index, turn, move, comment);
}

void Table::remove(const Index& index, bool confirm_promotion) {
  if(!confirm_promotion)
    if(m_movelist) m_movelist->remove(index);
  if(m_movelist_textual) m_movelist_textual->remove(index);
}

void Table::Table::promoteVariation(const Index& ix, int v) {
  if(m_movelist) m_movelist->promoteVariation(ix, v);
  //m_movelist_textual->promoteVariation(index);
}

void Table::select(const Index& index, bool confirm_promotion) {
  if(!confirm_promotion)
    if(m_movelist) m_movelist->select(index);
  if(m_movelist_textual) m_movelist_textual->select(index);
}

void Table::reset() {
  if(m_movelist) m_movelist->reset();
  if(m_movelist_textual) m_movelist_textual->reset();
}

Notifier* Table::getNotifier() {
  return m_movelist->getNotifier();
}

void Table::setNotifier(Notifier* n, bool detach_prev) {
  if(m_movelist) m_movelist->setNotifier(n, detach_prev);
  if(m_movelist_textual) m_movelist_textual->setNotifier(n, false); //false here, it is important
}

int Table::layoutStyle() {
  return m_movelist->layoutStyle();
}

void Table::setLayoutStyle(int x) {
  if(m_movelist) m_movelist->setLayoutStyle(x);
  if(m_movelist_textual) m_movelist_textual->setLayoutStyle(x);
}

//END Table--------------------------------------------------------------------

} //end namespace MoveList

