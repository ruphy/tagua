/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#ifndef VARIANTS_H
#define VARIANTS_H

#include <QString>
#include <map>
#include "tagua.h"
#include "fwd.h"

class Variants {
  class case_comparer {
  public:
    bool operator()(const QString& a, const QString& b) const {
      return a.compare(b, Qt::CaseInsensitive) < 0;
    }
  };
  
  typedef std::map<QString, VariantFactory*, case_comparer> Factories;
  Factories m_factories;
  
  Variants();
public:
  VariantPtr get(const QString& name) const;
  VariantFactory* getFactory(const QString& name) const;
  
  void addFactory(const QString& name, VariantFactory* factory);
  QStringList all() const;
  
  static Variants& instance();
};

#endif // VARIANTS_H
