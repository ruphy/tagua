/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "decoratedmove.h"

#include <iostream>
#include <QRegExp>

#include "common.h"

DecoratedMove::DecoratedMove() { }

DecoratedMove::DecoratedMove(const QString& str) {
  load(str);
}

void DecoratedMove::load(const QString& str) {
  static QRegExp figurine("\\{[^}]*\\}");
  int offset = 0;
  int begin;
  std::cout << "DecoratedMove load" << std::endl;
  while ((begin = figurine.indexIn(str, offset)) != -1) {
    std::cout << "begin = " << begin << std::endl;
    if (begin > offset) {
      m_elements.push_back(str.mid(offset, begin - offset));
      std::cout << "found text: " << str.mid(offset, begin - offset) << std::endl;
    }
    m_elements.push_back(MovePart(str.mid(begin + 1, figurine.matchedLength() - 2), MovePart::Figurine));
    std::cout << "found figurine: " << str.mid(begin + 1, figurine.matchedLength() - 2) << std::endl;
    offset = begin + figurine.matchedLength();
  }
  
  if (offset < str.length())
    m_elements.push_back(str.mid(offset));
}

void DecoratedMove::push_back(const MovePart& part) {
  m_elements.push_back(part);
}

MovePart DecoratedMove::operator[](int index) const {
  return m_elements[index];
}

unsigned int DecoratedMove::size() const {
  return m_elements.size();
}




