/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PREF_HIGHLIGHT_H
#define PREF_HIGHLIGHT_H

#include "ui/ui_pref_highlight.h"
#include "highlighter.h"
#include <boost/shared_ptr.hpp>
#include <QDialog>
#include <QColor>
#include <vector>

class ItemMapBackInserter : public PatternListAdaptorOutputIterator {
public:
  typedef std::map<QListWidgetItem*, Highlighter::Pattern> ItemMap;
private:
  ItemMap& m_map;
  QListWidget* m_list;
public:
  ItemMapBackInserter(ItemMap& map, QListWidget* list);
  virtual void add(const Highlighter::Pattern& p);
  QListWidgetItem* addItem(const Highlighter::Pattern& p);
};

class PrefHighlight : public QDialog
                    , private Ui::PrefHighlight {
Q_OBJECT
public:
  typedef ItemMapBackInserter::ItemMap ItemMap;
private:
  ItemMap m_patterns;
  boost::shared_ptr<Highlighter> m_highlighter;
  ItemMapBackInserter* m_inserter;
public:
  PrefHighlight();
  void apply();
  Highlighter::Pattern& currentPattern();
  void updateHighlighting();
public slots:
  void updateRegExp();
  void updateBold();
  void updateItalic();
  void updateColor(const QColor&);
  void changePattern(QListWidgetItem*);
  void editPatternName(QListWidgetItem* index);
  void addEntry();
  void removeEntry();
  void moveEntryUp();
  void moveEntryDown();
  void moveEntry(int);
};


#endif // PREF_HIGHLIGHT_H
