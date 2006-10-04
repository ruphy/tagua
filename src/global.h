/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef GLOBAL_H
#define GLOBAL_H

#include <memory>
#include <iostream>
#include <QDir>
#include "settings.h"
#include "foreach.hpp"
#undef foreach
#define foreach BOOST_FOREACH

class MasterSettings : public QObject
                     , public Settings {
Q_OBJECT
  QString m_filename;
  mutable QDomDocument m_doc;
protected:
  virtual QDomElement node() const;
public:
  MasterSettings(const QString& file);
  
  void onChange(QObject* receiver, const char* slot);
  void changed();
signals:
  void settingsChanged();
};

QString data_dir();

extern MasterSettings settings;

#endif // GLOBAL_H
