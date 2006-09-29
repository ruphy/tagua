/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <boost/shared_ptr.hpp>

class PatternListAdaptorOutputIterator;
class PatternListAdaptorInputIterator;

class Highlighter : public QSyntaxHighlighter {
Q_OBJECT
public:
  struct Pattern {
    QString name;
    QRegExp pattern;
    QTextCharFormat format;
  };
private:
  std::vector<Pattern> m_patterns;
public:
  Highlighter(QTextDocument* parent);
  Highlighter(QTextDocument* parent, const Pattern& pattern);
  void reload();
  
  static void load(PatternListAdaptorOutputIterator& out);
  static void save(const PatternListAdaptorInputIterator& in);
  
  virtual void highlightBlock(const QString& text);
  std::vector<Pattern>& patterns();
};


class PatternListAdaptorOutputIterator {
public:
  virtual ~PatternListAdaptorOutputIterator() { }
  virtual void add(const Highlighter::Pattern&) = 0;
};

class PatternListAdaptorInputIterator {
public:
  virtual ~PatternListAdaptorInputIterator() { }
  virtual boost::shared_ptr<PatternListAdaptorInputIterator> clone() const = 0;
  virtual bool valid() const = 0;
  virtual PatternListAdaptorInputIterator& inc() = 0;
  virtual const Highlighter::Pattern& value() const = 0;
};

#endif // HIGHLIGHTER_H
