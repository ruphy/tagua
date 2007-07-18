/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MASTERSETTINGS_H
#define MASTERSETTINGS_H

#include <memory>
#include <iostream>
#include <set>
#include <map>
#include <QDir>
#include "settings.h"
#include "foreach.hpp"
#undef foreach
#define foreach BOOST_FOREACH

class MasterSettings : public QObject
                     , public Settings {
Q_OBJECT

private:
  typedef std::set<std::pair<QObject*, const char*> > SlotSet;
  typedef std::map<int, SlotSet> SlotMap;

  SlotMap m_slots;
  QString m_filename;
  mutable QDomDocument m_doc;

private slots:
  void obj_destroyed(QObject* o);

protected:
  virtual QDomElement node() const;

public:
  MasterSettings(const QString& file);
  ~MasterSettings();

  void onChange(QObject* receiver, const char* slot, int priority = 0);
  void changed();
  void sync();
};

extern MasterSettings settings; // yes, I know global objects are evil

#endif // MASTERSETTINGS_H
