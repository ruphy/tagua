/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QFileDialog>

#include <iostream>
#include "qconnect.h"
#include "settings.h"

static const char* timeseal_cmd_tool_tip =
  "Expandable variables ref:\n"
  "  $(HOST_IP)   - The resolved ip address of the host\n"
  "  $(HOST_NAME) - The host name\n"
  "  $(PORT)      - The TCP port to connect to";

QConnect::QConnect(QWidget *parent, const char *name)
    : QDialog(parent)
{
  setObjectName(name);

  setupUi(this);
  connect(buttonTimesealPath, SIGNAL(clicked()), this, SLOT(setTimesealPath()));
  chkTimesealCmd->setToolTip(timeseal_cmd_tool_tip);
  editTimesealCmd->setToolTip(timeseal_cmd_tool_tip);
  //std::cout << "initializing dialog" << std::endl;

  if(settings["IcsUsername"])
    editUsername->setText(settings["IcsUsername"].value<QString>());
  if (settings["IcsPassword"]) {
    editPassword->setText(settings["IcsPassword"].value<QString>());
    chkStore->setChecked(true);
  }
  if (settings["IcsHost"])
    editHost->setText( settings["IcsHost"].value<QString>());
  spinPort->setValue( (settings["IcsPort"] | 5000).value<int>());
  groupTimeseal->setChecked( (settings["UseTimeseal"] |= false).value<bool>() );
  editTimeseal->setText( (settings["TimesealPath"] |= QString()).value<QString>());
  chkTimesealCmd->setChecked( (settings["UseTimesealCustomCmd"] |= false).value<bool>() );
  editTimesealCmd->setText(
    (settings["TimesealCustomCmd"] |= "$(HOST_IP) $(PORT)" ).value<QString>() );
}

void QConnect::setTimesealPath() {
  QString t = QFileDialog::getOpenFileName();
  if(!t.isEmpty())
    editTimeseal->setText(t);
}

void QConnect::accept() {
  QDialog::accept();

  settings["IcsUsername"] = editUsername->text();
  if (chkStore->isChecked())
    settings["IcsPassword"] = editPassword->text();
  else
    settings["IcsPassword"].remove();
  settings["IcsHost"] = editHost->text();
  settings["IcsPort"] = spinPort->value();
  settings["UseTimeseal"] = groupTimeseal->isChecked();
  settings["TimesealPath"] = editTimeseal->text();
  settings["UseTimesealCustomCmd"] = chkTimesealCmd->isChecked();
  settings["TimesealCustomCmd"] = editTimesealCmd->text();

  emit acceptConnection(editUsername->text(),
                        editPassword->text(),
                        editHost->text(),
                        spinPort->value(),
                        groupTimeseal->isChecked() ? editTimeseal->text() : QString(),
                        chkTimesealCmd->isChecked() ? editTimesealCmd->text() :
                        QString("$(HOST_IP) $(PORT)")
                      );
}

//#include "qconnect.moc"

#include "qconnect.moc"
