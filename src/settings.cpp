/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include "common.h"
#include "settings.h"

Settings settings(".kboardrc");

SettingRef::operator bool() {
  return settings->qSettings()->value(key).isValid();
}

void SettingRef::remove() {
  return settings->qSettings()->remove(key);
}

QSettings *Settings::qSettings()
{
  if(!m_qsettings) {
    QString base = QDir::homePath()+"/";
    m_qsettings = new QSettings( base+m_path, QSettings::IniFormat, qApp);
  }

  return m_qsettings;
}

void Settings::changed() {
  if(m_notifier) {
    emit m_notifier->settingsChanged();
  }
}

void Settings::onChange(QObject* obj, const char *slot) {
  if(!m_notifier)
    m_notifier = new SettingsNotifier;
  QObject::connect(m_notifier, SIGNAL(settingsChanged()), obj, slot);
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define DATA_INSTALL_DIR_STR TOSTRING(DATA_INSTALL_DIR)

static bool dir_ok(const QString& d) {
  return QFile::exists(d+"/icons")
          && QFile::exists(d+"/themes")
          && QFile::exists(d+"/scripts");
}

QString data_dir() {
  static bool initialied = false;
  static QString s_data_dir;

  if(initialied)
    return s_data_dir;

  if(settings["DataDir"])
    s_data_dir = settings["DataDir"].value<QString>();
  else {
    if(QFile::exists(DATA_INSTALL_DIR_STR)) {
      settings["DataDir"] = DATA_INSTALL_DIR_STR;
      s_data_dir = DATA_INSTALL_DIR_STR;
    }
    else if(dir_ok(QDir::current().absolutePath())) {
      settings["DataDir"] = QDir::current().absolutePath();
      s_data_dir = QDir::current().absolutePath();
    }
    else if(dir_ok(QDir::current().absolutePath()+"/..")) {
      settings["DataDir"] = QDir::cleanPath(QDir::current().absolutePath()+"/..");
      s_data_dir = QDir::cleanPath(QDir::current().absolutePath()+"/..");
    }
    else {
      QMessageBox::critical(NULL, "Error!", "KBoard could not find data!\n"
                                           "Please select kboard base directory");
      QString retv = QFileDialog::getExistingDirectory(NULL, "Select the data directory");
      if(retv.isEmpty() || !dir_ok(retv)) {
        if(!retv.isEmpty())
          QMessageBox::critical(NULL, "Error!", "This is not a valid data directory!\n"
                                                "KBoard data directory should have \"themes\",\n"
                                                "\"icons\" and \"scripts\" subdirectories");
        exit(1);
      }
      settings["DataDir"] = retv;
      s_data_dir = retv;
    }
  }
  initialied = true;
  return s_data_dir;
}

#include "settings.moc"
