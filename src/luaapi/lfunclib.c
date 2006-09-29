/*
  Copyright (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This file has been contributed by Rici Lake under the Lua licence:
  
  Copyright (c) 2006 Rici Lake
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to 
  deal in the Software without restriction, including without limitation the 
  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
  sell copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN 
  THE SOFTWARE. 
*/

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include "luaapi/lfunclib.h"

/* "Constructors" (or collectors, if you prefer). Take an iterator
 * and return a value.
 */

/* foreach just applies the self-function to each iteration tuple
 * in turn. The return the result is the last return value
 * of the function
 *
 * func:foreach(<iter>) ==>
 *
 * local rv...
 * for val... in <iter> do
 *   rv... = func(val...)
 * end
 * return rv...
 *
 */

static int f_foreach (lua_State *L) {
  lua_settop(L, 4);
  for (;;) {
    int callbase = lua_gettop(L);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_call(L, 2, LUA_MULTRET);
    if (lua_isnoneornil(L, callbase + 1)) {
      lua_settop(L, callbase);
      return callbase - 4;
    }
    lua_pushvalue(L, callbase + 1);
    lua_replace(L, 4);  /* ctl = newctl */
    lua_pushvalue(L, 1);
    if (callbase == 4) {
      lua_insert(L, 5);
    }
    else {
      lua_replace(L, 5);
      for (; callbase > 5; --callbase)
        lua_remove(L, callbase);
    }
    lua_call(L, lua_gettop(L) - 5, LUA_MULTRET);
  }
}

/* first calls the self-function on each iteration tuple in turn,
 * and returns the first true (not non-null) return of the self-function.

   func:first(<iter>) ==>
     for val... in <iter> do
       local rv... = func(val...)
       if (rv...) then return rv... end
     end
   end

   1: self_func  2: iter_func  3: base   4: control
   5: copy_self  6: returned control
 */

static int f_first (lua_State *L) {
  for (;;) {
    lua_settop(L, 4);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_call(L, 2, LUA_MULTRET);
    if (lua_isnoneornil(L, 6)) return 0;
    lua_pushvalue(L, 6);
    lua_replace(L, 4);
    lua_call(L, lua_gettop(L) - 5, LUA_MULTRET);
    if (lua_toboolean(L, 5)) return lua_gettop(L) - 5;
  }
}

/*
   func:fold1(init, <iter>)
     for val... in <iter> do
       init = func(init, val...)
     end
     return init
   end
   func:fold2(init, <iter>)
     for val, rest... in <iter> do
       init = func(init, rest...)
     end
     return init
   end

   1: self_func  2: init val  3: iter_func  4: base   5: control
   6: copy self  7: copy init 8: returned control
   6: returned init
 */

static int f_fold1 (lua_State *L) {
  lua_settop(L, 5);
  for (;;) {
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 5);
    lua_call(L, 2, LUA_MULTRET);
    if (lua_isnoneornil(L, 8)) { lua_settop(L, 7); return 1; }
    lua_pushvalue(L, 8);
    lua_replace(L, 5);
    lua_call(L, lua_gettop(L) - 6, 1);
    lua_replace(L, 2);
  }
}


static int f_fold2 (lua_State *L) {
  lua_settop(L, 5);
  for (;;) {
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, 3);
    lua_pushvalue(L, 4);
    lua_pushvalue(L, 5);
    lua_call(L, 2, LUA_MULTRET);
    if (lua_isnoneornil(L, 8)) { lua_settop(L, 7); return 1; }
    lua_pushvalue(L, 8);
    lua_replace(L, 5);
    lua_remove(L, 8);
    lua_call(L, lua_gettop(L) - 6, 1);
    lua_replace(L, 2);
  }
}


/* func:reduce{1,2}(default, <iter>) is like
 * func:fold{1,2}(init, <iter>)  except that the initial
 * value is taken from the first iteration of the loop.
 * If the loop fails to produce even one value, the default
 * argument is returned. Otherwise, default is not used.
 */

static int f_reduce1 (lua_State *L) {
  lua_settop(L, 5);
  lua_pushvalue(L, 3);
  lua_pushvalue(L, 4);
  lua_pushvalue(L, 5);
  lua_call(L, 2, 1);
  if (lua_isnil(L, -1)) { lua_settop(L, 2); return 1; }
  lua_pushvalue(L, -1);
  lua_replace(L, 2);
  lua_replace(L, 5);
  return f_fold1(L);
}

static int f_reduce2 (lua_State *L) {
  lua_settop(L, 5);
  lua_pushvalue(L, 3);
  lua_pushvalue(L, 4);
  lua_pushvalue(L, 5);
  lua_call(L, 2, 2);
  if (lua_isnil(L, -2)) { lua_settop(L, 2); return 1; }
  lua_replace(L, 2);
  lua_replace(L, 5);
  return f_fold2(L);
}

/* These ones could easily be implemented in terms of fold, but
 * the direct implementation uses fewer temporary objects.
 * Unlike the other stuff in this module, these are not
 * function methods; they're just functions in the collect
 * library.
 */

/* collect
 *   .keys => an array of the keys (or control variable, anyway)
 *   .vals => an array of the second iterator return
 *   .pairs => an table of (first) = (second)
 *   .keyset => a set of (first) = true
 *   .valset => a set of (second) = true
 */

static int c_advance_aux (lua_State *L, int iter, int nr, int cb) {
    lua_settop(L, cb - 1);
    lua_pushvalue(L, iter);
    lua_pushvalue(L, iter+1);
    lua_pushvalue(L, iter+2);
    lua_call(L, 2, nr);
    if (lua_isnoneornil(L, cb)) return 0;
    lua_pushvalue(L, cb);
    lua_replace(L, iter+2);
    return 1;
}

static int c_keys (lua_State *L) {
  int index = 0;
  lua_newtable(L);
  lua_insert(L, 1);
  while (c_advance_aux(L, 2, 1, 5))
    lua_rawseti(L, 1, ++index);
  lua_settop(L, 1);
  return 1;
}

static int c_vals (lua_State *L) {
  int index = 0;
  lua_newtable(L);
  lua_insert(L, 1);
  while (c_advance_aux(L, 2, 2, 5))
    lua_rawseti(L, 1, ++index);
  lua_settop(L, 1);
  return 1;
}

static int c_pairs (lua_State *L) {
  lua_newtable(L);
  lua_insert(L, 1);
  while (c_advance_aux(L, 2, 2, 5))
    lua_rawset(L, 1);
  lua_settop(L, 1);
  return 1;
}

static int c_keyset (lua_State *L) {
  lua_newtable(L);
  lua_insert(L, 1);
  while (c_advance_aux(L, 2, 1, 5)) {
    lua_pushboolean(L, 1);
    lua_rawset(L, 1);
  }
  lua_settop(L, 1);
  return 1;
}

static int c_valset (lua_State *L) {
  lua_newtable(L);
  lua_insert(L, 1);
  while (c_advance_aux(L, 2, 2, 5)) {
    lua_pushboolean(L, 1);
    lua_rawset(L, 1);
  }
  lua_settop(L, 1);
  return 1;
}


/* func:dropping(i) -> function
 * The returned function drops its ith argument (default 1) and
 * returns the application of func to the remaining arguments.
 * This is useful for removing keys from iterators in some of
 * these iterator methods
 */

static int f_dropping_aux (lua_State *L) {
  int i = lua_tointeger(L, lua_upvalueindex(2));
  int top = lua_gettop(L);
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_insert(L, 1);
  if (i <= top) { lua_remove(L, i + 1); --top; }
  lua_call(L, top, LUA_MULTRET);
  return lua_gettop(L);
}

static int f_dropping (lua_State *L) {
  luaL_checkany(L, 1);
  lua_settop(L, 2);
  if (lua_isnil(L, 2)) {
    lua_pushinteger(L, 1);
    lua_replace(L, 2);
  }
  else {
    luaL_checknumber(L, 2);
  }
  lua_pushcclosure(L, f_dropping_aux, 2);
  return 1;
}

/* Yuk. */
static int aux_nups (lua_State *L) {
  lua_Debug ar;
  lua_getstack(L, 0, &ar);
  lua_getinfo(L, "u", &ar);
  return ar.nups;
}


static int aux_insertupvalues (lua_State *L, int where) {
  int i;
  int nups = aux_nups(L);
  luaL_checkstack(L, nups, "insert upvalues");
  if (where > 0) {
    for (i = 1; i <= nups; ++i, ++where) {
      lua_pushvalue(L, lua_upvalueindex(i));
      lua_insert(L, where);
    }
  }
  else if (where < 0) {
    --where;
    for (i = 1; i <= nups; ++i) {
      lua_pushvalue(L, lua_upvalueindex(i));
      lua_insert(L, where);
    }
  }
  else {
    for (i = 1; i <= nups; ++i)
      lua_pushvalue(L, lua_upvalueindex(i));
  }
  return nups;
}

/* func:partial(...) -> function
 * The returned function is the original function with the given
 * arguments "filled in" from the left.
 */

static int f_partial_aux (lua_State *L) {
  aux_insertupvalues(L, 1);
  lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
  return lua_gettop(L);
}

static int f_partial (lua_State *L) {
  int top = lua_gettop(L);
  if (top >= LUAI_MAXUPVALUES) {
    luaL_error(L, "too many arguments, maximum is %d", LUAI_MAXUPVALUES);
  }
  if (lua_tocfunction(L, 1) == f_partial_aux) {
    int i = 1;
    /* We should find out how many there are, really. */
    luaL_checkstack(L, LUAI_MAXUPVALUES, "partial combination");
    for (; lua_getupvalue(L, 1, i); ++i) {
      lua_insert(L, i + 1);
    }
    top += i - 2;
  }
  lua_pushcclosure(L, f_partial_aux, top);
  return 1;
}

/* Building block */

static int f_reorder_aux (lua_State *L) {
  const char *format = lua_tostring(L, lua_upvalueindex(1));
  int nups = lua_tointeger(L, lua_upvalueindex(2));
  int top = lua_gettop(L);
  lua_pushvalue(L, lua_upvalueindex(3));  /* The function */
  for (;;) {
    switch (*format++) {
      case '%': {
        int base = *format++ - '0';
        int ch;
        for (ch = *format; '0' <= ch && ch <= '9'; ch = *++format)
          base = base * 10 + ch - '0';
        if (ch == '.') {
          while (*++format == '.');
          if (base <= nups) {
            luaL_checkstack(L, nups - base, "");
            for (; base <= nups; ++base)
              lua_pushvalue(L, lua_upvalueindex(base + 3));
          }
        }
        else if (base <= nups)
          lua_pushvalue(L, lua_upvalueindex(base));
        else
          lua_pushnil(L);
        break;
      }
      case '$': {
        /* duplicate above, but with top */
      }
      case '(': {
        int func = lua_gettop(L);
        /* recursive call */
        /* get count or ... */
        lua_call(L, lua_gettop(L) - func, nups);
        break;
      }
      case ')': {
        return;
      }
      case '\0': {
        return lua_gettop(L) - top;
      }
    }
  }
}



/* Functions which take iterators and return iterators */

/* for vals... in f:map(<iter>) do <block> end
 * ==>
 * for temp... in <iter> do
 *   local vals... = f(temp...)
 *   <block>
 * end
 */

static int f_map_aux (lua_State *L) {
  lua_settop(L, 0);
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_pushvalue(L, lua_upvalueindex(2));
  lua_pushvalue(L, lua_upvalueindex(3));
  lua_pushvalue(L, lua_upvalueindex(4));
  lua_call(L, 2, LUA_MULTRET);
  if (lua_isnoneornil(L, 2)) return 0;
  lua_pushvalue(L, 2);
  lua_replace(L, lua_upvalueindex(4));
  lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
  return lua_gettop(L);
}

static int f_map (lua_State *L) {
  lua_settop(L, 4);
  lua_pushcclosure(L, f_map_aux, 4);
  return 1;
}

/* Possibly more useful.
 * for k, vals... in f:mapvals(<iter>) do <block> end
 * ==>
 * for k, temp... in <iter> do
 *   local vals... = f(temp...)
 *   <block>
 * end
 */

static int f_mapvals_aux (lua_State *L) {
  lua_pushvalue(L, lua_upvalueindex(2));
  lua_insert(L, 1);
  lua_call(L, lua_gettop(L) - 1, LUA_MULTRET);
  if (lua_isnoneornil(L, 1)) return 0;
  lua_pushvalue(L, lua_upvalueindex(1));
  lua_insert(L, 2);
  lua_call(L, lua_gettop(L) - 2, LUA_MULTRET);
  return lua_gettop(L);
}

/* Factor this out */
static int f_mapvals (lua_State *L) {
  lua_settop(L, 4);
  lua_pushvalue(L, 1);
  lua_pushvalue(L, 2);
  lua_pushcclosure(L, f_mapvals_aux, 2);
  lua_replace(L, 2);
  return 3;
}


/* for vals... in f:filter(<iter>) do <block> end
 * ==>
 * for vals... in <iter> do
 *   if f(vals...) then
 *     <block>
 *   end
 * end
 */

static int f_filter_aux (lua_State *L) {
  int i, top;
  do {
    lua_settop(L, 2);
    lua_pushvalue(L, 1);
    lua_pushvalue(L, 2);
    lua_pushvalue(L, lua_upvalueindex(2));
    lua_replace(L, 2); /* Obj IterFunc Obj Ctl */
    lua_call(L, 2, LUA_MULTRET);
    top = lua_gettop(L);
    if (top == 1 || lua_isnil(L, 2)) return 0;
    luaL_checkstack(L, top, "filtered iterator returned too many results");
    lua_pushvalue(L, lua_upvalueindex(1));
    for (i = 2; i <= top; ++i) lua_pushvalue(L, i);
    lua_call(L, top - 1, 1);
  } while (!lua_toboolean(L, -1));
  lua_pop(L, 1);
  return top - 1;
}

static int f_filter (lua_State *L) {
  lua_settop(L, 4);
  lua_pushvalue(L, 1);
  lua_pushvalue(L, 2);
  lua_pushcclosure(L, f_filter_aux, 2);
  lua_replace(L, 2);
  return 3;
}

/* TODO:
 *   Things to add:
 *     pcall, coroutine.wrap -> both are appropriate function methods
 *     possibly some introspection stuff
 *     something like the new python with modifier: i.e. safely do a call
 *       with a prelude and postlude.
 */

static const luaL_reg func_funcs[] = {
  { "first", f_first },
  { "foreach", f_foreach },
  { "fold1", f_fold1 },
  { "fold2", f_fold2 },
  { "reduce1", f_reduce1 },
  { "reduce2", f_reduce2 },
  { "dropping", f_dropping },
  { "partial", f_partial },
  { "map", f_map },
  { "mapvals", f_mapvals },
  { "filter", f_filter },
  { NULL, NULL }
};

static const luaL_reg collect_funcs[] = {
  { "keys", c_keys },
  { "vals", c_vals },
  { "pairs", c_pairs },
  { "keyset", c_keyset },
  { "valset", c_valset },
  { NULL, NULL }
};

int luaopen_func (lua_State *L) {
  luaL_register(L, LUA_FUNCLIBNAME, func_funcs);
  lua_pushcfunction(L, luaopen_func); /* A function */
  if (!lua_getmetatable(L, -1)) {
    lua_createtable(L, 0, 0);
    lua_pushvalue(L, -3);
    lua_setfield(L, -2, "__index");
    lua_setmetatable(L, -2);
    lua_pop(L, 1);
  }
  else lua_pop(L, 2);
  return 1;
}

int luaopen_collect (lua_State *L) {
  luaL_register(L, LUA_COLLECTLIBNAME, collect_funcs);
  return 1;
}

