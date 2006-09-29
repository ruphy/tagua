/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LUAHL_H
#define LUAHL_H

#include <map>
#include <QRegExp>

class lua_State;
class HLine;

namespace LuaApi {

class Api {
  lua_State* m_state;
  
  void pcall(int nArgs, int nResults);
  void pushpair(int x, int y);
  std::pair<bool, HLine*> runEvent(const QString& text, int eventIndex, QRegExp& pattern);
public:
  Api();
  ~Api();

  lua_State* state() const { return m_state; }
  void runFile(const char* file);
  
  HLine* highlight(const QString& text);
  
};

} // namespace LuaHl


#endif // LUAHL_H
