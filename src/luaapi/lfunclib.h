/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LFUNCLIB_H
#define LFUNCLIB_H

#define LUA_FUNCLIBNAME    "func"
#define LUA_COLLECTLIBNAME "collect"

int luaopen_func (lua_State *L);
int luaopen_collect (lua_State *L);

#endif /*LFUNCLIB_H*/
