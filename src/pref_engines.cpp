/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "pref_engines.h"
#include <QLayout>
#include <KListWidget>
#include <QPushButton>
#include <KLineEdit>
#include <KUrlRequester>
#include <KComboBox>
#include <KMessageBox>
#include "foreach.h"
#include "mastersettings.h"

class EditEngine : public KDialog {
  KLineEdit* m_name;
  KComboBox* m_type;
  KUrlRequester* m_path;
  KUrlRequester* m_work_dir;
public:
  EditEngine(const QString& caption, QWidget* parent)
  : KDialog(parent) {
    setCaption(caption);
    setButtons(Ok | Cancel);
  
    QFrame* page = new QFrame(this);
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* tmp;
    QLabel* lbl;
    
    tmp = new QHBoxLayout;
    lbl = new QLabel(i18n("&Name:"), this);
    tmp->addWidget(lbl);
//     tmp->addStretch();
    m_name = new KLineEdit(this);
    lbl->setBuddy(m_name);
    tmp->addWidget(m_name);
    
    tmp->addStretch();    
    lbl = new QLabel(i18n("&Type:"), this);
    tmp->addWidget(lbl);
//     tmp->addStretch();
    m_type = new KComboBox(this);
    lbl->setBuddy(m_type);
    m_type->addItem(i18n("XBoard"), "xboard");
    m_type->addItem(i18n("UCI"), "UCI");
    m_type->addItem(i18n("Tagua (textual)"), "tagua-text");
    m_type->addItem(i18n("Tagua (DBUS)"), "tagua-dbus");
    m_type->addItem(i18n("GNU Shogi"), "gnushogi");
    tmp->addWidget(m_type);
    layout->addLayout(tmp);
    
    tmp = new QHBoxLayout;
    lbl = new QLabel(i18n("&Path:"), this);
    tmp->addWidget(lbl);
//     tmp->addStretch();
    m_path = new KUrlRequester(this);
    lbl->setBuddy(m_path);
    tmp->addWidget(m_path);
    layout->addLayout(tmp);
    
    tmp = new QHBoxLayout;
    lbl = new QLabel(i18n("&Work directory:"), this);
    tmp->addWidget(lbl);
//     tmp->addStretch();
    m_work_dir = new KUrlRequester(this);
    lbl->setBuddy(m_work_dir);
    tmp->addWidget(m_work_dir);
    layout->addLayout(tmp);

    page->setLayout(layout);    
    setMainWidget(page);
    
    m_name->setFocus();
  }
  
  void load(const EngineDetails& e) {
    m_name->setText(e.name);
    m_type->setCurrentIndex(e.type);
    m_path->setPath(e.path);
    m_work_dir->setPath(e.workPath);
  }
  
  bool validate() {
    if (m_name->text().isEmpty()) {
      KMessageBox::error(this, i18n("Choose a name for the engine"), i18n("No engine name"));
      return false;
    }
    return true;  
  }
  
  void save(EngineDetails& e) {
    if (validate()) {
      e.name = m_name->text();
      e.type = static_cast<EngineDetails::EngineType>(m_type->currentIndex());
      e.path = m_path->url().path();
      e.workPath = m_work_dir->url().path();
    }
  }
};

PrefEngines::PrefEngines(const QString&, QWidget* parent)
: QWidget(parent) {
  QHBoxLayout* layout = new QHBoxLayout;
  
  m_list = new QListWidget(this);
  layout->addWidget(m_list);
  
  QVBoxLayout* buttons = new QVBoxLayout;
  m_new = new QPushButton(KIcon("edit-add"), "&New...", this);
  buttons->addWidget(m_new);
  m_edit = new QPushButton(KIcon("edit"), "&Edit...", this);
  buttons->addWidget(m_edit);
  m_delete = new QPushButton(KIcon("edit-delete"), "&Delete", this);
  buttons->addWidget(m_delete);
  buttons->addStretch();
  layout->addLayout(buttons);
    
  setLayout(layout);
  
  connect(m_new, SIGNAL(pressed()), this, SLOT(new_engine()));
  connect(m_edit, SIGNAL(pressed()), this, SLOT(edit_engine()));
  connect(m_delete, SIGNAL(pressed()), this, SLOT(delete_engine()));
  connect(m_list, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(edit_engine()));
  
  // load engines
  foreach (Settings s, settings().group("engines").array("engine")) {
    EngineDetails e;
    e.load(s);
    m_engines.append(e);
  }
  reload_engines();
  
  connect(m_list, SIGNAL(currentRowChanged(int)), this, SLOT(update_buttons()));
  update_buttons();
}

PrefEngines::~PrefEngines() { }

void PrefEngines::apply() {
  SettingArray engines = settings().group("engines").newArray("engine");
  foreach (EngineDetails e, m_engines) {
    e.save(engines.append());
  }
}

void PrefEngines::reload_engines() {
  m_list->clear();
  foreach (EngineDetails e, m_engines) {
    m_list->addItem(e.name);
  }
}

void PrefEngines::new_engine() {
  EditEngine dialog("New engine", this);
  dialog.exec();

  EngineDetails e;
  dialog.save(e);
  m_engines.append(e);
  
  reload_engines();
}

void PrefEngines::edit_engine() {
  int row = m_list->currentRow();
  Q_ASSERT(row != -1);
  EditEngine dialog("Edit engine", this);
  
  EngineDetails& e = m_engines[m_list->currentRow()];
  dialog.load(e);
  dialog.exec();
  dialog.save(e);
  
  reload_engines();
}

void PrefEngines::delete_engine() {
  int row = m_list->currentRow();
  Q_ASSERT(row != -1);
  
  m_engines.removeAt(row);
  reload_engines();
}

void PrefEngines::update_buttons() {
  bool sel = m_list->currentRow() != -1;
  m_edit->setEnabled(sel);
  m_delete->setEnabled(sel);
}
