/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef QCONNECT_H
#define QCONNECT_H

#include "ui_quickconnect.h"

class QConnect: public QDialog, public Ui::QuickConnect {
Q_OBJECT
public:
  QConnect(QWidget *parent = 0, const char *name = 0);

public slots:
  virtual void accept();
  void setTimesealPath();

signals:
  void acceptConnection(const QString&, const QString&,
                        const QString&, quint16,
                        const QString&, const QString&);
};

#endif
