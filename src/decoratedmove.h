/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef DECORATEDMOVE_H
#define DECORATEDMOVE_H

#include <QString>
#include <QList>

class MovePart {
public:
  enum Type {
    Text,
    Figurine
  };

  QString m_string;
  Type    m_type;

  MovePart(const QString& s, Type t = Text)
    : m_string(s), m_type(t) {}
};

class DecoratedMove {
  typedef QList<MovePart> ElementList;
  
  ElementList m_elements;
public:
  DecoratedMove();
  explicit DecoratedMove(const QString& str);
  void load(const QString& str);
  
  MovePart operator[](int index) const;
  unsigned int size() const;
  
  // deprecated methods
  void push_back(const MovePart& part);
};



#endif //DECORATEDMOVE_H
