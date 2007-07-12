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
#include "global.h"

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

  Settings s_ics = settings.group("ics");
  if (s_ics["username"])
    editUsername->setText(s_ics["username"].value<QString>());
  if (s_ics["password"]) {
    editPassword->setText(s_ics["password"].value<QString>());
    chkStore->setChecked(true);
  }
  if (s_ics["host"])
    editHost->setText(s_ics["host"] | "");
  spinPort->setValue((s_ics["port"] | 5000));
  Settings s_timeseal = s_ics.group("timeseal");
  groupTimeseal->setChecked(s_timeseal.flag("use", false));
  editTimeseal->setText(s_timeseal["path"] |= QString());
  chkTimesealCmd->setChecked(s_timeseal["command"].flag("use", false));  editTimesealCmd->setText(s_timeseal["command"] |= "$(HOST_IP) $(PORT)");
}

void QConnect::setTimesealPath() {
  QString t = QFileDialog::getOpenFileName();
  if(!t.isEmpty())
    editTimeseal->setText(t);
}

void QConnect::accept() {
  QDialog::accept();

  Settings s_ics = settings.group("ics");
  s_ics["username"] = editUsername->text();
  if (chkStore->isChecked())
    s_ics["password"] = editPassword->text();
  else
    s_ics["password"].remove();
  s_ics["host"] = editHost->text();
  s_ics["port"] = spinPort->value();

  {
    Settings s_timeseal = s_ics.group("timeseal");
    s_timeseal.setFlag("use", groupTimeseal->isChecked());
    s_timeseal["path"] = editTimeseal->text();
    s_timeseal.group("command").setFlag("use", chkTimesealCmd->isChecked());
    s_timeseal["command"] = editTimesealCmd->text();
  }

  emit acceptConnection(editUsername->text(),
                        editPassword->text(),
                        editHost->text(),
                        spinPort->value(),
                        groupTimeseal->isChecked() ? editTimeseal->text() : QString(),
                        chkTimesealCmd->isChecked() ? editTimesealCmd->text() :
                        QString("$(HOST_IP) $(PORT)")
                      );
}


