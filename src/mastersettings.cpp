/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "mastersettings.h"
#include "common.h"
#include <QTextStream>
#include <KStandardDirs>

QDomElement MasterSettings::node() const {
  if (m_node.isNull()) {
    QFile f(m_filename);
    if (!f.open(QFile::ReadOnly | QFile::Text) || !m_doc.setContent(&f)) {
      ERROR("Unable to open configuration file for reading.");

//       // create a stub configuration file
//       {
//         QFile stub(m_filename);
//         if (!stub.open(QFile::WriteOnly | QFile::Text)) {
//           std::cout << "Unable to write to configuration file." << std::endl;
//           exit(1);
//         }
//         QTextStream stream(&stub);
//         stream << "<?xml version=\"1.0\"?>\n"
//                   "<configuration>\n"
//                   "</configuration>\n";
//       }
//
//       // reopen it
//       if (!f.open(QFile::ReadOnly | QFile::Text))
//         exit(1);

      m_doc.appendChild(m_doc.createElement("configuration"));
//      std::cout << m_doc.toString() << std::endl;
    }


    const_cast<QDomElement&>(m_node) = m_doc.documentElement();
    Q_ASSERT(!m_node.isNull());
    Q_ASSERT(!m_node.ownerDocument().isNull());
  }

  return m_node;
}

MasterSettings::MasterSettings() {
  m_filename = KStandardDirs::locateLocal("config", "taguarc.xml");
}

MasterSettings::MasterSettings(const QString& filename) {
  m_filename = KStandardDirs::locateLocal("config", filename);
}

MasterSettings::~MasterSettings() {
  sync();
}

void MasterSettings::setupObserver(Observer& observer) {
  connect(observer.object, SIGNAL(destroyed(QObject*)), this, SLOT(objectDestroyed(QObject*)));
}

void MasterSettings::onChange(QObject* obj, const char* method) {
  m_observers.push_front(Observer(obj, method, 0));
  setupObserver(m_observers.front());
}

void MasterSettings::onChange(QObject* obj, const char* method, const char* dependency) {
  if (dependency) {
    // go backwards through all existing observers, searching for something
    // on which we depend.
    for (ObserverList::reverse_iterator it = m_observers.rbegin();
        it != m_observers.rend(); ++it) {
      const char* observer_class = it->object->metaObject()->className();
      if (observer_class && strcmp(observer_class, dependency) == 0) {
        // we hit a dependency wall: we can't be notified
        // before *it, so add the new Observer just here
        ObserverList::iterator us = m_observers.insert(it.base(), Observer(obj, method, dependency));
        setupObserver(*us);
        
        // now check for a cyclic dependency
        const char* this_class = obj->metaObject()->className();
        for (ObserverList::iterator it2 = m_observers.begin(); it2 != us; ++it2) {
          if (it2->dependency && strcmp(it2->dependency, this_class) == 0) {
            // something which is notified before has us a dependency
            // this means that there is a cyclic dependency it2 -> us -> it.
            WARNING("Removing a cyclic dependency: " <<
              it2->object->metaObject()->className() << " -> " <<
              this_class << " -> " <<
              observer_class);
              
            // remove the cycle
            it2->dependency = 0;
          }
        }
        
        // done
        return;
      }
    }
  }
  
  // no dependency
  onChange(obj, method);
}

void MasterSettings::sync() {
  // store to file
  QFile f(m_filename);
  if (!f.open(QFile::WriteOnly | QFile::Text))
    ERROR("Cannot open configuration file for writing");
  else {
    QTextStream stream(&f);
    stream << node().ownerDocument().toString();
  }

}

void MasterSettings::objectDestroyed(QObject* obj) {
  for (ObserverList::iterator it = m_observers.begin();
       it != m_observers.end();) {
    if (it->object == obj) {
      it = m_observers.erase(it);
    }
    else {
      ++it;
    }
  }
}

void MasterSettings::changed() {
  foreach (Observer& observer, m_observers) {
    observer.object->metaObject()->invokeMethod(observer.object, observer.method, Qt::DirectConnection);
  }
  sync();
}

QString MasterSettings::filename() const {
  return m_filename;
}

MasterSettings& settings() {
  static MasterSettings static_settings;
  return static_settings;
}


