/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
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
#include <list>
#include <QDir>

#include "settings.h"

class MasterSettings : public QObject
                     , public Settings {
Q_OBJECT
public:
  enum LookupType {
    PathLookup,
    StandardDirsLookup
  };
private:
  struct Observer {
    QObject* object;
    const char* dependency;
    const char* method;
    
    Observer(QObject* obj, const char* meth, const char* dep)
    : object(obj)
    , dependency(dep)
    , method(meth) { }
  };
  typedef std::list<Observer> ObserverList;

  ObserverList m_observers;
  QString m_filename;
  mutable QDomDocument m_doc;
  
  void setupObserver(Observer&);
private Q_SLOTS:
  void objectDestroyed(QObject* o);

protected:
  virtual QDomElement node() const;

public:
  MasterSettings(const QString& file, LookupType lookup = StandardDirsLookup);
  
  /**
    * Bind to the local configuration file.
    */
  MasterSettings();
  ~MasterSettings();

  /**
    * Set up an observer to be notified whenever settings change.
    * \param observer The object to be notified.
    * \param method The callback method for the notification, specified as a C string.
    * \note @a method should be just the method name (no parentheses, no SLOT macro).
    */
  void onChange(QObject* observer, const char* method);
  
  /**
    * Set up an observer to be notified whenever settings change.
    * \param observer The object to be notified.
    * \param method The callback method for the notification, specified as a C string.
    * \param dependency A C string representing a type (inheriting from QObject). Objects
    *                   of this type will be notified before @a observer.
    * \note @a method should be just the method name (no parentheses, no SLOT macro).
    */
  void onChange(QObject* observer, const char* method, const char* dependency);

  void changed();
  void sync();
  QString filename() const;
};

MasterSettings& settings();

#endif // MASTERSETTINGS_H
