/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "engineinfo.h"
#include "xboardengine.h"
#include "gnushogiengine.h"
#include "ui.h"
#include <iostream>

using namespace boost;

void EngineInfo::playAsWhite() {
  m_ui.addPlayingEngine(0, engine());
}

void EngineInfo::playAsBlack() {
  m_ui.addPlayingEngine(1, engine());
}

void EngineInfo::analyze() {
  if (m_token.valid()) {
    m_ui.removeAnalysingEngine(m_token);
    m_token = EntityToken();
  }
  else
    m_token = m_ui.addAnalysingEngine(engine());
}

EngineInfo::EngineInfo(const EngineDetails& details, UI& ui)
: QObject(&ui)
, m_details(details)
, m_ui(ui) { }

shared_ptr<Engine> EngineInfo::engine() {
  shared_ptr<Engine> res;
  if (m_details.type == EngineDetails::XBoard)
    res = shared_ptr<Engine>(new XBoardEngine(m_details.path, QStringList()));
  else if (m_details.type == EngineDetails::GNUShogi)
    res = shared_ptr<Engine>(new GNUShogiEngine(m_details.path, QStringList()));
  else {
    ERROR("Unimplemented engine type " << EngineDetails::typeName(m_details.type));
    return shared_ptr<Engine>();
  }

  if (res)
    res->setWorkingPath(m_details.workPath);

  return res;
}
