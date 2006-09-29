/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
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

class VariantInfo;

class Variant {
public:
  class case_comparer {
  public:
    bool operator()(const QString& a, const QString& b) {
      return a.compare(b, Qt::CaseInsensitive) < 0;
    }
  };

  typedef std::map<QString, VariantInfo*, case_comparer> Variants;

  static bool s_initialized;
  static Variants s_registered;

  static void register_variant(VariantInfo* v);
  template<typename T>
  static void register_variant();
  static void initialize();

  static VariantInfo* variant(const QString& name);
  static const Variants& allVariants();
  //static VariantInfo* variant(Code code);
};

#endif // VARIANTS_H
