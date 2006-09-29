/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "highlighter.h"
#include "settings.h"

using namespace boost;

class BackInserterPatternIteratorAdaptor : public PatternListAdaptorOutputIterator {
  std::vector<Highlighter::Pattern>& m_vector;
public:
  BackInserterPatternIteratorAdaptor(std::vector<Highlighter::Pattern>& vector)
  : m_vector(vector) { }
  
  virtual void add(const Highlighter::Pattern& p) {
    m_vector.push_back(p);
  }
};

#if 0
class VectorIterator : public PatternListAdaptorInputIterator {
  const std::vector<Highlighter::Pattern>::const_iterator m_end;
  std::vector<Highlighter::Pattern>::const_iterator m_it;
public:
  VectorIterator(const std::vector<Highlighter::Pattern>& vector)
  : m_end(vector.end())
  , m_it(vector.begin()) { }

  virtual shared_ptr<PatternListAdaptorInputIterator> clone() const {
    return shared_ptr<PatternListAdaptorInputIterator>(new VectorIterator(*this));
  }
  
  virtual bool valid() const {
    return m_it != m_end;
  }
  
  virtual PatternListAdaptorInputIterator& inc() {
    ++m_it;
    return *this;
  }
  
  virtual const Highlighter::Pattern& value() const {
    return *m_it;
  }
};
#endif

Highlighter::Highlighter(QTextDocument* parent)
: QSyntaxHighlighter(parent) {
  reload();
}

Highlighter::Highlighter(QTextDocument* parent, const Pattern& pattern)
: QSyntaxHighlighter(parent) {
  m_patterns.push_back(pattern);
}


void Highlighter::load(PatternListAdaptorOutputIterator& out) {
  QSettings* config = settings.qSettings();
  config->beginGroup("Highlighting");
  int size = config->beginReadArray("patterns");
  for (int i = 0; i < size; i++) {
    config->setArrayIndex(i);
    Pattern pattern;
    pattern.name = settings["name"].value<QString>();
    pattern.pattern = QRegExp(settings["pattern"].value<QString>());
    if (settings["foreground"])
      pattern.format.setForeground(settings["foreground"].value<QColor>());
    if (settings["background"])
      pattern.format.setBackground(settings["background"].value<QColor>());
    if (settings["bold"].value<bool>())
      pattern.format.setFontWeight(QFont::Bold);
    if (settings["italic"].value<bool>())
      pattern.format.setFontItalic(true);
    
    out.add(pattern);
  }
  config->endArray();
  config->endGroup();
}

void Highlighter::save(const PatternListAdaptorInputIterator& in) {
  QSettings* config = settings.qSettings();
  config->beginGroup("Highlighting");
  config->beginWriteArray("patterns");
  int i = 0;
  for (shared_ptr<PatternListAdaptorInputIterator> it = in.clone();
       it->valid(); it->inc(), ++i) {
    const Pattern& pattern = it->value();
    config->setArrayIndex(i);
    settings["name"] = pattern.name;
    settings["pattern"] = pattern.pattern.pattern();
    settings["foreground"] = pattern.format.foreground().color();
    settings["bold"] = pattern.format.fontWeight() == QFont::Bold;
    settings["italic"] = pattern.format.fontItalic();    
  }
  config->endArray();
  config->endGroup();
}

void Highlighter::reload() {
  BackInserterPatternIteratorAdaptor out(m_patterns);
  load(out);
}

std::vector<Highlighter::Pattern>& Highlighter::patterns() {
  return m_patterns;
}

void Highlighter::highlightBlock(const QString& text) {
  int offset = 0;
  
  while (true) {
    int best = -1;
    int best_offset = -1;
    int best_length;
    
    // try each pattern and find the closest matching one
    for (uint i = 0; i < m_patterns.size(); i++) {
      int match_offset = text.indexOf(m_patterns[i].pattern, offset);
      if (match_offset >= 0 && 
          (best_offset == -1 || match_offset < best_offset)) {
        int length = m_patterns[i].pattern.matchedLength();
        if (length > 0) {
          best = i;
          best_offset = match_offset;
          best_length = length;
        }
      }
    }
    
    if (best >= 0) {
      setFormat(best_offset, best_length, m_patterns[best].format);
      offset = best_offset + best_length;
    }
    else break;
  }
}
#include "highlighter.moc"
