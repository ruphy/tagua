/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>
#include <QPointer>
#include <iostream>
#include "common.h"

class Settings;

class SettingValueRef {
public:
  QVariant variant;
  SettingValueRef(const QVariant& v) : variant(v){}
  template <typename T> void operator>>(T&);
  template<typename T> T value(){ return variant.value<T>(); }
};

class SettingRef {
public:
  Settings *settings;
  QString key;
  SettingRef(Settings* s, const QString& k)
    : settings(s), key(k) {}
  template<typename T> void operator>>(T&);
  template<typename T> void operator=(const T& t);
  template<typename T> SettingValueRef operator|(const T& t);
  template<typename T> SettingValueRef operator|=(const T& t);
  template<typename T> T value();
  operator bool();
  void remove();
};

/**
  * Helper class to notify settings changes
  */
class SettingsNotifier : public QObject {
Q_OBJECT

friend class Settings;

signals:
  void settingsChanged();
};

class Settings {
  QPointer<QSettings> m_qsettings;
  QPointer<SettingsNotifier> m_notifier;
  const char *m_path;

public:
  QSettings *qSettings();

  Settings(const char* path) : m_qsettings(NULL), m_path(path) {}
  ~Settings() {
    if(m_qsettings)
      delete static_cast<QSettings*>(m_qsettings);
    if(m_notifier)
      delete static_cast<SettingsNotifier*>(m_notifier);
  }
  SettingRef operator[](const QString& k){
    return SettingRef(this, k);
  }
  void onChange(QObject* obj, const char *slot);
  void changed();
};

template <typename T>
void SettingValueRef::operator>>(T& t) {
  if(!variant.template canConvert<T>()) {
    std::cout << " --> Error in SettingValueRef::operator T(), cannot cast value!" << std::endl;
    std::cout << "     got a " << variant.typeName() << " while expecting a " <<
                                                  wrap_cptr(QVariant(T()).typeName()) << std::endl;
  }
  t = variant.value<T>();
}

template <typename T>
void SettingRef::operator>>(T& t) {
  t = value<T>();
}

template<typename T>
void SettingRef::operator=(const T& t) {
  settings->qSettings()->setValue(key, t);
}

template<typename T>
SettingValueRef SettingRef::operator|(const T& t) {
  return SettingValueRef(settings->qSettings()->value(key, QVariant(t)));
}

template<typename T>
SettingValueRef SettingRef::operator|=(const T& t) {
  QVariant variant = settings->qSettings()->value(key);
  if(!variant.isValid()) {
    variant = QVariant(t);
    settings->qSettings()->setValue(key, variant);
  }
  return SettingValueRef(variant);
}

template<typename T>
T SettingRef::value() {
  QVariant variant = settings->qSettings()->value(key);
  if(!variant.template canConvert<T>()) {
    std::cout << " --> Error in SettingRef::value<T>(), cannot cast value in key " << key << std::endl;
    std::cout << "     got a " << wrap_cptr(variant.typeName()) << " while expecting a " <<
                                            wrap_cptr(QVariant(T()).typeName()) << std::endl;
  }
  return variant.value<T>();
}

QString data_dir();

extern Settings settings;

#endif //SETTINGS_H

