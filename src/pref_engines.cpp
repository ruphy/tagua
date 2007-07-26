/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "pref_engines.h"

#include <KIcon>

#include "foreach.h"
#include "mastersettings.h"


class no_engine { };

PrefEngines::PrefEngines(const QString&, QWidget* parent)
: QWidget(parent) {
  setupUi(this);

  connect(m_engine_list, SIGNAL(itemChanged(QListWidgetItem*)), this, SLOT(setName(QListWidgetItem*)));
  connect(m_engine_list, SIGNAL(currentItemChanged(QListWidgetItem*, QListWidgetItem*)),
    this, SLOT(changeEngine(QListWidgetItem*)));
  connect(m_add_engine, SIGNAL(clicked()), this, SLOT(add()));
  connect(m_remove_engine, SIGNAL(clicked()), this, SLOT(remove()));
  connect(m_path, SIGNAL(textChanged(const QString&)), this, SLOT(setPath()));
  connect(m_type, SIGNAL(currentIndexChanged(int)), this, SLOT(setType()));
  connect(m_workdir, SIGNAL(textChanged(const QString&)), this, SLOT(setWorkDir()));

  m_add_engine->setIcon(KIcon("add"));
  m_remove_engine->setIcon(KIcon("edit-delete"));

  SettingArray s_engines = settings().group("engines").array("engine");
  foreach (Settings s, s_engines) {
    EngineDetails details;
    s["name"] >> details.name;
    s["path"] >> details.path;
    details.type = EngineDetails::typeFromName(s["type"].value<QString>());
    if (s["workPath"]) s["workPath"] >> details.workPath;

    addEngine(details);
  }
}

EngineDetails& PrefEngines::currentEngine() {
  QListWidgetItem* item = m_engine_list->currentItem();
  if (!item) throw no_engine();
  return m_engines[item];
}

PrefEngines::~PrefEngines() { }

QListWidgetItem* PrefEngines::addEngine(const EngineDetails& details) {
  QListWidgetItem* item = new QListWidgetItem(details.name, m_engine_list);
  m_engines[item] = details;
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  m_type->setCurrentIndex(0);
  return item;
}

void PrefEngines::add() {
  QListWidgetItem* item = addEngine(EngineDetails());
  m_engine_list->editItem(item);
}

void PrefEngines::remove() {
  QListWidgetItem* item = m_engine_list->currentItem();
  Q_ASSERT(item);
  m_engines.erase(item);
  delete item;
}

void PrefEngines::setName(QListWidgetItem* item) {
  try {
    if (item) currentEngine().name = item->text();
  }
  catch (no_engine&) { }
}

void PrefEngines::setPath() {
  try {
    currentEngine().path = m_path->url().prettyUrl();
  }
  catch (no_engine&) { }
}

void PrefEngines::setType() {
  try {
    currentEngine().type = static_cast<EngineDetails::EngineType>(m_type->currentIndex());
  }
  catch (no_engine&) { }
}

void PrefEngines::setWorkDir() {
  try {
    currentEngine().workPath = m_workdir->url().prettyUrl();
  }
  catch (no_engine&) { }
}

void PrefEngines::changeEngine(QListWidgetItem*) {
  try {
    m_path->setUrl(currentEngine().path);
    m_workdir->setUrl(currentEngine().workPath);
    m_type->setCurrentIndex(currentEngine().type);
  }
  catch (no_engine&) { }
}


void PrefEngines::apply() {
  SettingArray s_engines = settings().group("engines").newArray("engine");
  std::pair<QListWidgetItem*, EngineDetails> p;

  for (int i = 0; i < m_engine_list->count(); i++) {
    EngineDetails& details = m_engines[m_engine_list->item(i)];
    Settings s = s_engines.append();
    s["name"] = details.name;
    s["path"] = details.path;
    s["type"] = EngineDetails::typeName(details.type);
    if (!details.workPath.isEmpty())
      s["workPath"] = details.workPath;
  }
}

