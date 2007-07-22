/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "themeinfo.h"

#include <QHash>
#include <QDir>
#include <QFileInfo>
#include <KDesktopFile>

#include "foreach.h"
#include "settings.h"

ThemeInfo::operator bool() const {
  return !file_name.isEmpty();
}

void ThemeInfo::save(Settings& s) {
  s["file-name"]     = file_name;
  s["name"]          = name;
  s["description"]   = description;
  {
    SettingArray s_variants = s.group("variants").newArray("variant");
    foreach (QString v, variants) {
      Settings s_var = s_variants.append();
      s_var["name"] = v;
    }
  }
  s["last-modified"] = last_modified.toString();
  s["desktop-file"] = desktopFile;
}

ThemeInfo ThemeInfo::fromSettings(const Settings& s) {
  ThemeInfo res;

  s["file-name"] >> res.file_name;
  s["name"] >> res.name;
  s["description"] >> res.description;
  s["desktop-file"] >> res.desktopFile;
  res.last_modified = QDateTime::fromString(s["last-modified"].value<QString>());

  SettingArray variants = s.group("variants").array("variant");
  foreach (Settings var, variants) {
    res.variants.append(var["name"].value<QString>());
  }

  return res;
}

ThemeInfo ThemeInfo::fromDesktopFile(const QString& desktop_file) {
  ThemeInfo res;

  QFileInfo desktop_file_info(desktop_file);
  res.desktopFile = desktop_file;
  if (!KDesktopFile::isDesktopFile(desktop_file)) {
    return res;
  }

  KDesktopFile theme(desktop_file);
  KConfigGroup themeData = theme.desktopGroup();

  res.name = theme.readName();
  res.description = theme.readComment();
  res.description.replace("|", "\n");
  res.variants = themeData.readEntry("X-Tagua-Variants").split(QRegExp("\\s*,\\s*"));
  res.file_name = themeData.readEntry("X-Tagua-Script");
  if (res.file_name.isEmpty()) {
    // use a file with the same name as the .desktop, but with .lua extension
    res.file_name = desktop_file;
    res.file_name.replace(QRegExp("\\.desktop$"), ".lua");
  }
  else {
    if (!QDir::isAbsolutePath(res.file_name)) {
    // the file is relative to the .desktop directory
      res.file_name = QDir(desktop_file_info.path()).filePath(res.file_name);
    }
  }
  res.last_modified = desktop_file_info.lastModified();

  return res;
}

uint qHash(const ThemeInfo& info) {
  return qHash(info.file_name);
}

bool ThemeInfo::operator==(const ThemeInfo& info) const {
  return desktopFile == info.desktopFile;
}
