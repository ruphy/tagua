/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <QRegExp>
#include "pgnparser.h"
#include "common.h"

QRegExp PGN::number("^(\\d+)(?:(?:\\.\\s+)?(\\.\\.\\.)|\\.?)?");
QRegExp PGN::begin_var("^\\(");
QRegExp PGN::end_var("^\\)");
QRegExp PGN::comment("^\\{[^}]*\\}");
QRegExp PGN::comment2("^;[^\\n]*\\n");
QRegExp PGN::wsPattern("^\\s+");
QRegExp PGN::tag("^\\[(\\S+)\\s+\"((?:[^\"]|\\\\\")*)\"\\]");
QRegExp PGN::move_tag("^\\$(\\d+)");
QRegExp PGN::move("^[^$\\{\\(\\[\\s][^\\{\\(\\[\\s]*");
QRegExp PGN::result("^(?:\\*|1-0|0-1|1/2-1/2)");
QRegExp PGN::time("^\\([\\d:.]*\\)");
QRegExp PGN::eol("(?:[ \t]\r?\n\r?|\r?\n\r?[ \t]|\r?\n\r?)");

bool PGN::tryRegExp(QRegExp& re, const QString& str, int& offset) {
  if (re.indexIn(str, offset, QRegExp::CaretAtOffset) != -1) {
    offset += re.matchedLength();
    return true;
  }
  else return false;
}

#define IGNORE(re) if (tryRegExp((re), pgn, offset)) continue;
bool PGN::parse(const QString& pgn) {
  int offset = 0;

  while (offset < pgn.length()) {
    IGNORE(wsPattern);

    // read result
    if (result.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      QString res = pgn.mid(offset, result.matchedLength());
      offset += result.matchedLength();
      return true;
    }

    IGNORE(comment2);
    IGNORE(time);

    // read comment
    if (tag.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      m_tags[tag.cap(1)] = tag.cap(2);
      offset += tag.matchedLength();
      continue;
    }

    // read comment
    if (comment.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      m_entries.push_back(pgn.mid(offset + 1, comment.matchedLength() - 2).replace(eol, " "));
      offset += comment.matchedLength();
      continue;
    }

    // read var, 1
    if (begin_var.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      m_entries.push_back(BeginVariation());
      offset += begin_var.matchedLength();
      continue;
    }

    // read var, 2
    if (end_var.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      m_entries.push_back(EndVariation());
      offset += end_var.matchedLength();
      continue;
    }

    // read move tag
    if (move_tag.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      // not nothing :)
      tryRegExp(wsPattern, pgn, offset);
    }

    int num = 0;

    // read number, and do not continue
    if (number.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      num = number.cap(1).toInt()*2 + (number.cap(2).isEmpty() ? 0 : 1);
      offset += number.matchedLength();
      tryRegExp(wsPattern, pgn, offset);
    }

    // read move
    if (move.indexIn(pgn, offset, QRegExp::CaretAtOffset) != -1) {
      m_entries.push_back(Move(num, move.cap(0)));
      offset += move.matchedLength();
      continue;
    }

    // parse error!
    std::cout << "pgn parse error! at" << pgn.mid(offset, 100) << std::endl;
    return false;
  }

  return true;
}
#undef IGNORE

PGN::PGN(const QString& str) {
  m_valid = parse(str);
}
