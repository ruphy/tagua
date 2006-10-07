/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "global.h"
#include <QTextStream>

QDomElement MasterSettings::node() const {
  if (m_node.isNull()) {
    QFile f(m_filename);
    if (!f.open(QFile::ReadOnly | QFile::Text)) {
      std::cout << "Unable to open configuration file for reading." << std::endl;

      // create a stub configuration file
      {
        QFile stub(m_filename);
        if (!stub.open(QFile::WriteOnly | QFile::Text)) {
          std::cout << "Unable to write to configuration file." << std::endl;
          exit(1);
        }
        QTextStream stream(&stub);
        stream << "<?xml version=\"1.0\"?>\n"
                  "<configuration>\n"
                  "</configuration>\n";
      }

      // reopen it
      if (!f.open(QFile::ReadOnly | QFile::Text))
        exit(1);
    }

    m_doc.setContent(&f);
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

void MasterSettings::onChange(QObject* obj, const char* slot) {
  connect(this, SIGNAL(settingsChanged()), obj, slot);
}

void MasterSettings::sync() {
  // store to file
  QFile f(m_filename);
  if (!f.open(QFile::WriteOnly | QFile::Text))
    std::cout << "ERROR: cannot open configuration file for writing" << std::endl;
  else {
    QTextStream stream(&f);
    stream << node().ownerDocument().toString();
  }

}

void MasterSettings::changed() {
  emit settingsChanged();
  sync();
}

MasterSettings settings(".kboardrc.xml");

QString data_dir() {
  return "/home/monge/kboard/";
}

#include "global.moc"

