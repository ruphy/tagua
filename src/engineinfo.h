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

struct EngineDetails {
  enum EngineType {
    XBoard,
    UCI,
    TaguaText,
    TaguaDBUS,
    Unknown
  };

  QString name;
  QString path;
  EngineType type;
  QString workPath;

  EngineDetails(const QString& name, const QString& path, const EngineType& type)
  : name(name), path(path), type(type) { }
  EngineDetails() { }

  static QString typeName(EngineType t) {
    switch (t) {
    case XBoard:
      return "xboard";
    case UCI:
      return "UCI";
    case TaguaText:
      return "tagua-text";
    case TaguaDBUS:
      return "tagua-dbus";
    default:
      return "unknown";
    }
  }

  static EngineType typeFromName(const QString& name) {
    if (name == "xboard")
      return XBoard;
    else if (name == "UCI")
      return UCI;
    else if (name == "tagua-text")
      return TaguaText;
    else if (name == "tagua-dbus")
      return TaguaDBUS;
    else return Unknown;
  }
};

class EngineInfo : public QObject {
Q_OBJECT
  EngineDetails m_details;
  UI& m_ui;
  EntityToken m_token;
protected:
  virtual boost::shared_ptr<Engine> engine();
public:
  EngineInfo(const EngineDetails& details, UI& ui);
  void setWorkPath(const QString& wp) { m_details.workPath = wp; }
public Q_SLOTS:
  void playAsWhite();
  void playAsBlack();
  void analyze();
};

#endif // ENGINEINFO_H
