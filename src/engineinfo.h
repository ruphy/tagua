/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ENGINEINFO_H
#define ENGINEINFO_H

#include <QString>
#include <QObject>
#include <boost/shared_ptr.hpp>
#include "controllers/entitytoken.h"

class Engine;
class UI;

class EngineInfo : public QObject {
Q_OBJECT
  QString m_name;
  QString m_path;
  QString m_type;
  UI& m_ui;
  EntityToken m_token;
protected:
  virtual boost::shared_ptr<Engine> engine();
public:
  EngineInfo(const QString& name, const QString& path, const QString& type, UI& ui);
  QString m_workPath;
public slots:
  void playAsWhite();
  void playAsBlack();
  void analyze();
};

#endif // ENGINEINFO_H
