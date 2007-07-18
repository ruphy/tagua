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

MasterSettings::MasterSettings(const QString& filename)
: m_filename(QDir(QDir::homePath()).filePath(filename)) { }

MasterSettings::~MasterSettings() {
  sync();
}

void MasterSettings::onChange(QObject* obj, const char* slot, int prority) {
  std::cout << "register " << obj << " " << slot << std::endl;
  m_slots[prority].insert( std::pair<QObject*, const char*>(obj, slot) );
  connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(obj_destroyed(QObject*)));
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

void MasterSettings::obj_destroyed(QObject* obj) {
  for (SlotMap::iterator it = m_slots.begin(); it != m_slots.end(); /*++it*/ ) {
    SlotSet::iterator j = it->second.lower_bound( std::pair<QObject*, const char*>(obj, NULL) );
    while(j->first == obj)
      it->second.erase(j++);

    if(it->second.empty())
      m_slots.erase(it++);
    else
      ++it;
  }
}

void MasterSettings::changed() {
  for (SlotMap::iterator it = m_slots.begin(); it != m_slots.end(); ++it )
  for (SlotSet::iterator j = it->second.begin(); j != it->second.end(); ++j ) {

    //yes, moc is shit.
    char* mystr = (char*)alloca(strlen(j->second)+1);
    if(isdigit(j->second[0]))
      strcpy(mystr, j->second+1);
    else
      strcpy(mystr, j->second);

    if(char* par = index(mystr, '('))
      *par = '\0';
    if(char* par = index(mystr, ' '))
      *par = '\0';

    bool r = j->first->metaObject()->invokeMethod( j->first, mystr, Qt::DirectConnection);
  }
  sync();
}

MasterSettings settings(".taguarc.xml");


