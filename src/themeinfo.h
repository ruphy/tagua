/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef THEMEINFO_H
#define THEMEINFO_H

#include <QStringList>
#include <QDateTime>

class Settings;

struct ThemeInfo {
  QString file_name;
  QString name;
  QString description;
  QString desktopFile;
  QStringList variants;
  QDateTime last_modified;
            
  operator bool() const;
  
  bool operator==(const ThemeInfo& info) const;
  
  void save(Settings& s);
  static ThemeInfo fromSettings(const Settings& s);
};

uint qHash(const ThemeInfo& info);
  
#endif // THEMEINFO_H

