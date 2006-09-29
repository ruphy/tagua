/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "pref_highlight.h"
#include <QSyntaxHighlighter>
#include <QColorDialog>
#include <iostream>
#include "common.h"

using namespace boost;

//BEGIN ItemMapBackInserter ---------------------------------------------------

ItemMapBackInserter::ItemMapBackInserter(ItemMap& map, QListWidget* list)
: m_map(map)
, m_list(list) { }

void ItemMapBackInserter::add(const Highlighter::Pattern& p) {
  addItem(p);
}

QListWidgetItem* ItemMapBackInserter::addItem(const Highlighter::Pattern& p) {
  QListWidgetItem* item = new QListWidgetItem(p.name, m_list);
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  m_map[item] = p;
  return item;
}

//END ItemMapBackInserter -----------------------------------------------------


//BEGIN MapIterator------------------------------------------------------------

class MapIterator : public PatternListAdaptorInputIterator {
  typedef ItemMapBackInserter::ItemMap ItemMap;
  const ItemMap& m_item_map;
  const QListWidget* m_widget_list;
  int m_index;
public:
  MapIterator(const ItemMap& map, const QListWidget* w)
  : m_item_map(map)
  , m_widget_list(w)
  , m_index(0) { }

  virtual shared_ptr<PatternListAdaptorInputIterator> clone() const {
    return shared_ptr<PatternListAdaptorInputIterator>(new MapIterator(*this));
  }

  virtual bool valid() const {
    return m_index>=0 && m_index<m_widget_list->count();
  }

  virtual PatternListAdaptorInputIterator& inc() {
    ++m_index;
    return *this;
  }

  virtual const Highlighter::Pattern& value() const {
    ItemMap::const_iterator it = m_item_map.find(m_widget_list->item(m_index));
    if(it == m_item_map.end()) {
      Q_ASSERT(false);
      return *(Highlighter::Pattern*)NULL;
    }
    else
      return it->second;
  }
};

//END MapIterator--------------------------------------------------------------


//BEGIN PrefHighlight----------------------------------------------------------

PrefHighlight::PrefHighlight() {
  setupUi(this);

  m_inserter = new ItemMapBackInserter(m_patterns, m_pattern_list);

  connect(m_pattern_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
          this, SLOT(changePattern(QListWidgetItem*)));
  connect(m_bold, SIGNAL(stateChanged(int)),
          this, SLOT(updateBold()));
  connect(m_italic, SIGNAL(stateChanged(int)),
          this, SLOT(updateItalic()));
  connect(m_pattern, SIGNAL(textChanged(const QString&)),
          this, SLOT(updateRegExp()));
  connect(m_color, SIGNAL(changed(const QColor&)),
          this, SLOT(updateColor(const QColor&)));
  connect(m_pattern_list, SIGNAL(itemChanged(QListWidgetItem*)),
          this, SLOT(editPatternName(QListWidgetItem*)));
  connect(m_add, SIGNAL(clicked()),
          this, SLOT(addEntry()));
  connect(m_delete, SIGNAL(clicked()),
          this, SLOT(removeEntry()));
  connect(m_up, SIGNAL(clicked()),
          this, SLOT(moveEntryUp()));
  connect(m_down, SIGNAL(clicked()),
          this, SLOT(moveEntryDown()));

  Highlighter::load(*m_inserter);
  m_pattern_list->setCurrentRow(0);
}

void PrefHighlight::apply() {
  std::cout << "size: " << m_patterns.size() << std::endl;
  MapIterator it(m_patterns, m_pattern_list);
  Highlighter::save(it);
}

void PrefHighlight::changePattern(QListWidgetItem* item) {
  Highlighter::Pattern& pattern = m_patterns[item];
  m_pattern->setText(pattern.pattern.pattern());
  m_bold->setChecked(pattern.format.fontWeight() == QFont::Bold);
  m_italic->setChecked(pattern.format.fontItalic());
  m_color->setColor(pattern.format.foreground().color());

  updateRegExp();
}

void PrefHighlight::updateRegExp() {
  currentPattern().pattern = QRegExp(m_pattern->text());
  updateHighlighting();
}

void PrefHighlight::updateHighlighting() {
  Highlighter::Pattern pattern = currentPattern();
  if (pattern.pattern.isEmpty()) {
    m_highlighter.reset();
  }
  else {
    if (m_highlighter)
      m_highlighter->patterns()[0] = pattern;
    else
      m_highlighter = shared_ptr<Highlighter>(new Highlighter(m_test->document(), pattern));
  }
  QString text = m_test->toPlainText();
  m_test->clear();
  m_test->setPlainText(text);
}

void PrefHighlight::updateBold() {
  currentPattern().format.setFontWeight(m_bold->isChecked() ? QFont::Bold : QFont::Normal);
  updateHighlighting();
}

void PrefHighlight::updateItalic() {
  currentPattern().format.setFontItalic(m_italic->isChecked());
  updateHighlighting();
}

void PrefHighlight::updateColor(const QColor& color) {
  currentPattern().format.setForeground(color);
  updateHighlighting();
}

Highlighter::Pattern& PrefHighlight::currentPattern() {
  return m_patterns[m_pattern_list->currentItem()];
}

void PrefHighlight::editPatternName(QListWidgetItem* item) {
  QString name = item->text();
  m_patterns[item].name = name;
}

void PrefHighlight::addEntry() {
  QListWidgetItem* item = m_inserter->addItem(Highlighter::Pattern());
  m_pattern_list->setCurrentItem(item);
  m_pattern_list->editItem(item);
}

void PrefHighlight::removeEntry() {
  QListWidgetItem* item = m_pattern_list->currentItem();
  m_patterns.erase(item);
  delete item;
}

void PrefHighlight::moveEntry(int index) {
  QListWidgetItem* item = m_pattern_list->takeItem(m_pattern_list->currentRow());
  m_pattern_list->insertItem(index, item);
  m_pattern_list->setCurrentRow(index);
}

void PrefHighlight::moveEntryUp() {
  int index = m_pattern_list->currentRow();
  if (index > 0) {
    moveEntry(index - 1);
  }
}

void PrefHighlight::moveEntryDown() {
  int index = m_pattern_list->currentRow();
  if (index < m_pattern_list->count() - 1) {
    moveEntry(index + 1);
  }
}

//END PrefHighlight------------------------------------------------------------

#include "pref_highlight.moc"
