/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef PGNPARSER_H
#define PGNPARSER_H

#include <vector>
#include <map>
#include <iosfwd>
#ifdef Q_CC_MSVC
  #pragma warning( push )
  #pragma warning( disable : 4100 )
  #include <boost/variant.hpp>
  #pragma warning( pop )
#else
  #include <boost/variant.hpp>
#endif
#include <QString>
#include "algebraicnotation.h"

class QRegExp;

class PGN {
public:
  class Move {
  public:
    int m_number;
    QString m_move;
    Move(int n, const QString& s)
      : m_number(n), m_move(s) { }
  };
  class BeginVariation {};
  class EndVariation {};
  typedef boost::variant<Move, QString, BeginVariation, EndVariation> Entry;

private:
/*  friend std::ostream& operator<<(std::ostream& os, const PGN& pgn);*/
  QString m_result;
  bool m_valid;

  static QRegExp number, begin_var, end_var, comment, comment2,
                   wsPattern, tag, result, time, eol, move_tag, move;

  static bool tryRegExp(QRegExp& re, const QString& str, int& offset);
  bool parse(const QString& pgn);
public:
  std::vector<Entry> m_entries;
  std::map<QString, QString> m_tags;
  explicit PGN(const QString&);
  inline bool valid() const { return m_valid; }
  inline uint size() const { return m_entries.size(); }
  const Entry* operator[](int index) const { return &m_entries[index]; }
};

#endif // PGNPARSER_H
