/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LUAAPI__GENERICWRAPPER_H
#define LUAAPI__GENERICWRAPPER_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "lfunclib.h"
}
#include <iostream>
#include <QString>
#include "common.h"

namespace Loader { class Context; }

namespace LuaApi {

#define WRAP_0(func, selftype, restype) \
  static int func(lua_State* l) { \
    selftype* self = retrieve(l, 1); \
    CAT0(lua_push, restype)(l, self->func()); \
    return 1; \
  }

#define PROPERTY_RW(PROP, GET, SET, restype)          \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, retrieve(l, 1, DontCheck)->GET());      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    retrieve(l, 1, DontCheck)->SET( lua_to##restype(l, 2) );     \
    return 0;                                         \
  }

#define PROPERTY_RW_TYPE(PROP, GET, SET, restype, TYPE)     \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, retrieve(l, 1, DontCheck)->GET());      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    retrieve(l, 1, DontCheck)->SET( TYPE(lua_to##restype(l, 2)) );     \
    return 0;                                         \
  }

#define PROPERTY_RW_CLASS(PROP, GET, SET, CLASS)      \
  static int get_##PROP(lua_State* l) {               \
    Wrapper<CLASS>::allocate(l, new CLASS(retrieve(l, 1, DontCheck)->GET()));      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    retrieve(l, 1, DontCheck)->SET( *Wrapper<CLASS>::retrieve(l, 2, AssertOk) );     \
    return 0;                                         \
  }

#define PROPERTY_RO(PROP, GET, restype)               \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, retrieve(l, 1, DontCheck)->GET());      \
    return 1;                                         \
  }

#define P_PROPERTY_RW(PROP, GET, SET, restype)          \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, (*retrieve(l, 1, DontCheck))->GET());      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    (*retrieve(l, 1, DontCheck))->SET( lua_to##restype(l, 2) );     \
    return 0;                                         \
  }

#define P_PROPERTY_RW_TYPE(PROP, GET, SET, restype, TYPE)     \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, (*retrieve(l, 1, DontCheck))->GET());      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    (*retrieve(l, 1, DontCheck))->SET( TYPE(lua_to##restype(l, 2)) );     \
    return 0;                                         \
  }

#define P_PROPERTY_RO_TYPE(PROP, GET, restype, TYPE)     \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, (*retrieve(l, 1, DontCheck))->GET());      \
    return 1;                                         \
  }

#define P_PROPERTY_RW_CLASS(PROP, GET, SET, CLASS)      \
  static int get_##PROP(lua_State* l) {               \
    Wrapper<CLASS>::allocate(l, new CLASS((*retrieve(l, 1, DontCheck))->GET()));      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    (*retrieve(l, 1, DontCheck))->SET( *Wrapper<CLASS>::retrieve(l, 2, AssertOk) );     \
    return 0;                                         \
  }

#define P_PROPERTY_RO_CLASS(PROP, GET, CLASS)      \
  static int get_##PROP(lua_State* l) {               \
    Wrapper<CLASS>::allocate(l, new CLASS((*retrieve(l, 1, DontCheck))->GET()));      \
    return 1;                                         \
  }

#define P_PROPERTY_RW_P_CLASS(PROP, GET, SET, CLASS)      \
  static int get_##PROP(lua_State* l) {               \
    Wrapper<CLASS>::allocate(l, (*retrieve(l, 1, DontCheck))->GET());      \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    (*retrieve(l, 1, DontCheck))->SET( *Wrapper<CLASS>::retrieve(l, 2, AssertOk) );     \
    return 0;                                         \
  }

#define P_PROPERTY_RO_P_CLASS(PROP, GET, CLASS)      \
  static int get_##PROP(lua_State* l) {               \
    Wrapper<CLASS>::allocate(l, (*retrieve(l, 1, DontCheck))->GET());      \
    return 1;                                         \
  }

#define P_PROPERTY_RO(PROP, GET, restype)               \
  static int get_##PROP(lua_State* l) {               \
    lua_push##restype(l, (*retrieve(l, 1, DontCheck))->GET());      \
    return 1;                                         \
  }

#define P_PROPERTY_RO_QSTRING(PROP, GET)               \
  static int get_##PROP(lua_State* l) {               \
    QString s = (*retrieve(l, 1, DontCheck))->GET();  \
    lua_pushstring(l, s.toAscii().constData());   \
    return 1;                                         \
  }

#define P_PROPERTY_RW_QSTRING(PROP, GET, SET)         \
  static int get_##PROP(lua_State* l) {               \
    QString s = (*retrieve(l, 1, DontCheck))->GET();  \
    lua_pushstring(l, s.toAscii().constData());   \
    return 1;                                         \
  }                                                   \
                                                      \
  static int set_##PROP(lua_State* l) {               \
    (*retrieve(l, 1, DontCheck))->SET( QString(lua_tostring(l, 2)) );     \
    return 0;                                         \
  }

#define SET_PROPERTY_RW(LUA, PROP)                    \
    set_property(LUA, #PROP, get_##PROP, set_##PROP);

#define SET_PROPERTY_RO(LUA, PROP)                    \
    set_property(LUA, #PROP, get_##PROP, NULL);


#define LOADING_CONTEXT   "kboard_loading_context"
#define CURRENT_DIRECTORY "kboard_script_curr_dir"
#define API_LOADER        "kboard_api_loader"




class StackCheck {
  lua_State* l;
  int top;
public:
  StackCheck(lua_State* l, int expect = 0) : l(l) {
    top = lua_gettop(l) + expect;
  }

  ~StackCheck() {
    if (lua_gettop(l) != top) {
      std::cout << "Wrong stack size!\n\texpected = " << top <<
                   "\n\tactual = " << lua_gettop(l) << std::endl;
    }
  }
};



enum CheckStrenght {
  DontCheck,
  Check,
  AssertOk
};


template <typename T>
class Wrapper;


//BEGIN GenericWrappeBase------------------------------------------------------

class GenericWrapperBase {
public:
  static QString file_path(lua_State* l, const QString& f);

  static Loader::Context* retrieve_context(lua_State* l);

  /**
    * Add property f to the table at @a index.
    */
  static void set_property(lua_State* l, const char* name, lua_CFunction get,
                                            lua_CFunction set = NULL, int index = -1);

  /**
    * Add method f to the table at @a index.
    */
  static void set_method(lua_State* l, lua_CFunction f, const char* name, int index = -1);

  /**
    * Add meta method f to the table at @a index.
    */
  static void set_meta_method(lua_State* l, lua_CFunction f, const char* name, int index = -1);

  /**
    * Object index read event
    */
  static int object_meta_index_event(lua_State* l);

  /**
    * Object index write event
    */
  static int object_meta_newindex_event(lua_State* l);
};

//END GenericWrapperBase-------------------------------------------------------


//BEGIN GenericWrapper---------------------------------------------------------

template <typename T>
class GenericWrapper : public GenericWrapperBase {
public:

  static void register_class(lua_State* l);

  /**
    * Retrieve a wrapped object on the stack.
    */
  static T* retrieve(lua_State* l, int index = -1, CheckStrenght check = Check);

  /**
    * Destroy a wrapped object of type T.
    */
  static int deallocate(lua_State* l);

  /**
    * Wrap an object of type T in a Lua userdatum.
    * Lua takes ownership of the object, so that it is
    * automatically disposed by the Lua garbage collector.
    */
  static void allocate(lua_State* l, T* x);

  /**
    * Create the factory class and static metatable.
    */
  static void create_prototype(lua_State* l, const char* prototype_name);

  /**
    * Calls the Wrapper&lt;T&gt; constructor
    */
  static int constructor_wrapper(lua_State* l);

  /**
    * Pushes this object's meta table.
    */
  static void push_meta_table(lua_State* l);
};

template<typename T>
void GenericWrapper<T>::register_class(lua_State* l) {
  create_prototype(l, Wrapper<T>::class_name());
}

template<typename T>
T* GenericWrapper<T>::retrieve(lua_State* l, int index, CheckStrenght check) {
  StackCheck s(l);

  if(check != DontCheck) {
    if(!lua_getmetatable(l,index)) {
      if(check == AssertOk)
        luaL_error(l, "Mismatch, got object of unknown type expecting %s",
                                                    Wrapper<T>::class_name());
      return NULL;
    }
    GenericWrapper<T>::push_meta_table(l);

    if(!lua_rawequal(l, -2, -1)) {
      if(check == AssertOk) {
        if(lua_istable(l, -2)) {
          lua_getfield(l, -2, ".type");
          if(lua_isstring(l, -1))
            luaL_error(l, "Mismatch, got object of type %s expecting %s",
                          wrap_cptr(lua_tostring(l,-1)), Wrapper<T>::class_name());
        }
        lua_pop(l,2);
        luaL_error(l, "Mismatch, got object of unknown type expecting %s",
                                                    Wrapper<T>::class_name());
      }
      lua_pop(l,2);
      return NULL;
    }
    lua_pop(l,2);
  }

  T** data = reinterpret_cast<T**>(
    lua_touserdata(l, index));
  return data ? *data : NULL;
}

template<typename T>
int GenericWrapper<T>::deallocate(lua_State* l) {
  StackCheck s(l, -1);

  T* data = retrieve(l);
  lua_pop(l, 1);
  delete data;
  return 0;
}

template<typename T>
void GenericWrapper<T>::allocate(lua_State* l, T* x) {
  StackCheck s(l, 1);

  // create userdata
  T** data = reinterpret_cast<T**>(
    lua_newuserdata(l, sizeof(T*)));
  *data = x;

  // set metatable
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
void GenericWrapper<T>::create_prototype(lua_State* l, const char* prototype_name) {
  StackCheck s(l);

  lua_newtable(l); // factory class

  lua_newtable(l); // metatable
  lua_pushcfunction(l, &constructor_wrapper);
  lua_setfield(l, -2, "__call");
  lua_setmetatable(l, -2);

  lua_newtable(l); // object metatable
    // add gc metamethod
    lua_pushcfunction(l, &deallocate);
    lua_setfield(l, -2, "__gc");

    // add index metamethod
    lua_pushcfunction(l, &object_meta_index_event);
    lua_setfield(l, -2, "__index");

    // add newindex metamethod
    lua_pushcfunction(l, &object_meta_newindex_event);
    lua_setfield(l, -2, "__newindex");

    lua_newtable(l);
    lua_setfield(l, -2, ".methods");

    lua_newtable(l);
    lua_setfield(l, -2, ".get");

    lua_newtable(l);
    lua_setfield(l, -2, ".set");

    lua_pushstring(l, prototype_name);
    lua_setfield(l, -2, ".type");

    Wrapper<T>::create_index_table(l);
  lua_setfield(l, -2, "object_meta_table");

  lua_setglobal(l, prototype_name);
}

template<typename T>
int GenericWrapper<T>::constructor_wrapper(lua_State* l) {
  lua_remove(l, 1);
  return Wrapper<T>::constructor(l);
}

template<typename T>
void GenericWrapper<T>::push_meta_table(lua_State* l) {
  StackCheck s(l, 1);

  lua_getglobal(l, Wrapper<T>::class_name());
  lua_pushstring(l, "object_meta_table");
  lua_rawget(l,-2);
  lua_remove(l,-2);
}

//END GenericWrapper-----------------------------------------------------------

//BEGIN GenericWrapperByValue---------------------------------------------------------

template <typename T>
class GenericWrapperByValue : public GenericWrapperBase {
public:

  static void register_class(lua_State* l);

  /**
    * Retrieve a wrapped object on the stack.
    */
  static T* retrieve(lua_State* l, int index = -1, CheckStrenght check = Check);

  /**
    * Destroy a wrapped object of type T.
    */
  static int deallocate(lua_State* l);

  /**
    * Wrap an object of type T in a Lua userdatum.
    * Lua takes ownership of the object, so that it is
    * automatically disposed by the Lua garbage collector.
    */
  static void allocate(lua_State* l, const T x);

  static void create(lua_State* l);
  template<typename A1>
  static void create(lua_State* l, const A1&);
  template<typename A1, typename A2>
  static void create(lua_State* l, const A1&, const A2&);
  template<typename A1, typename A2, typename A3>
  static void create(lua_State* l, const A1&, const A2&, const A3&);
  template<typename A1, typename A2, typename A3, typename A4>
  static void create(lua_State* l, const A1&, const A2&, const A3&, const A4&);
  template<typename A1, typename A2, typename A3, typename A4, typename A5>
  static void create(lua_State* l, const A1&, const A2&, const A3&, const A4&, const A5&);

  /**
    * Create the factory class and static metatable.
    */
  static void create_prototype(lua_State* l, const char* prototype_name);

  /**
    * Calls the Wrapper&lt;T&gt; constructor
    */
  static int constructor_wrapper(lua_State* l);

  /**
    * Pushes this object's meta table.
    */
  static void push_meta_table(lua_State* l);
};

template<typename T>
void GenericWrapperByValue<T>::register_class(lua_State* l) {
  create_prototype(l, Wrapper<T>::class_name());
}

template<typename T>
T* GenericWrapperByValue<T>::retrieve(lua_State* l, int index, CheckStrenght check) {
  StackCheck s(l);

  if(check != DontCheck) {
    if(!lua_getmetatable(l,index)) {
      if(check == AssertOk)
        luaL_error(l, "Mismatch, got object of unknown type expecting %s",
                                                    Wrapper<T>::class_name());
      return NULL;
    }
    GenericWrapperByValue<T>::push_meta_table(l);

    if(!lua_rawequal(l, -2, -1)) {
      if(check == AssertOk) {
        if(lua_istable(l, -2)) {
          lua_getfield(l, -2, ".type");
          if(lua_isstring(l, -1))
            luaL_error(l, "Mismatch, got object of type %s expecting %s",
                          wrap_cptr(lua_tostring(l,-1)), Wrapper<T>::class_name());
        }
        lua_pop(l,2);
        luaL_error(l, "Mismatch, got object of unknown type expecting %s",
                                                    Wrapper<T>::class_name());
      }
      lua_pop(l,2);
      return NULL;
    }
    lua_pop(l,2);
  }

  T* data = reinterpret_cast<T*>(
    lua_touserdata(l, index));
  return data;
}

template<typename T>
int GenericWrapperByValue<T>::deallocate(lua_State* l) {
  StackCheck s(l, -1);

  T* data = retrieve(l);
  lua_pop(l, 1);
  data->~T(); //placement delete
  return 0;
}

template<typename T>
void GenericWrapperByValue<T>::allocate(lua_State* l, T x) {
  StackCheck s(l, 1);

  // create userdata
  T* data = reinterpret_cast<T*>(
    lua_newuserdata(l, sizeof(T)));
  new(data) T(x); //placement new

  StackCheck check(l);

  // set metatable
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
void GenericWrapperByValue<T>::create(lua_State* l) {
  StackCheck s(l, 1);

  T* data = reinterpret_cast<T*>(lua_newuserdata(l, sizeof(T)));
  new(data) T(); //placement new
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
template<typename A1>
void GenericWrapperByValue<T>::create(lua_State* l, const A1& a1) {
  StackCheck s(l, 1);

  T* data = reinterpret_cast<T*>(lua_newuserdata(l, sizeof(T)));
  new(data) T(a1); //placement new
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
template<typename A1, typename A2>
void GenericWrapperByValue<T>::create(lua_State* l, const A1& a1, const A2& a2) {
  StackCheck s(l, 1);

  T* data = reinterpret_cast<T*>(lua_newuserdata(l, sizeof(T)));
  new(data) T(a1, a2); //placement new
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
template<typename A1, typename A2, typename A3>
void GenericWrapperByValue<T>::create(lua_State* l, const A1& a1, const A2& a2, const A3& a3) {
  StackCheck s(l, 1);

  T* data = reinterpret_cast<T*>(lua_newuserdata(l, sizeof(T)));
  new(data) T(a1, a2, a3); //placement new
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
template<typename A1, typename A2, typename A3, typename A4>
void GenericWrapperByValue<T>::create(lua_State* l, const A1& a1, const A2& a2,
                                 const A3& a3, const A4& a4) {
  StackCheck s(l, 1);

  T* data = reinterpret_cast<T*>(lua_newuserdata(l, sizeof(T)));
  new(data) T(a1, a2, a3, a4); //placement new
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
template<typename A1, typename A2, typename A3, typename A4, typename A5>
void GenericWrapperByValue<T>::create(lua_State* l, const A1& a1, const A2& a2, const A3& a3,
                                                const A4& a4, const A5& a5) {
  StackCheck s(l, 1);

  T* data = reinterpret_cast<T*>(lua_newuserdata(l, sizeof(T)));
  new(data) T(a1, a2, a3, a4, a5); //placement new
  push_meta_table(l);
  lua_setmetatable(l, -2);
}

template<typename T>
void GenericWrapperByValue<T>::create_prototype(lua_State* l, const char* prototype_name) {
  StackCheck s(l);

  lua_newtable(l); // factory class

  lua_newtable(l); // metatable
  lua_pushcfunction(l, &constructor_wrapper);
  lua_setfield(l, -2, "__call");
  lua_setmetatable(l, -2);

  lua_newtable(l); // object metatable
    // add gc metamethod
    lua_pushcfunction(l, &deallocate);
    lua_setfield(l, -2, "__gc");

    // add index metamethod
    lua_pushcfunction(l, &object_meta_index_event);
    lua_setfield(l, -2, "__index");

    // add newindex metamethod
    lua_pushcfunction(l, &object_meta_newindex_event);
    lua_setfield(l, -2, "__newindex");

    lua_newtable(l);
    lua_setfield(l, -2, ".methods");

    lua_newtable(l);
    lua_setfield(l, -2, ".get");

    lua_newtable(l);
    lua_setfield(l, -2, ".set");

    lua_pushstring(l, prototype_name);
    lua_setfield(l, -2, ".type");

    Wrapper<T>::create_index_table(l);
  lua_setfield(l, -2, "object_meta_table");

  lua_setglobal(l, prototype_name);
}

template<typename T>
int GenericWrapperByValue<T>::constructor_wrapper(lua_State* l) {
  lua_remove(l, 1);
  return Wrapper<T>::constructor(l);
}

template<typename T>
void GenericWrapperByValue<T>::push_meta_table(lua_State* l) {
  StackCheck s(l, 1);

  lua_getglobal(l, Wrapper<T>::class_name());
  lua_pushstring(l, "object_meta_table");
  lua_rawget(l,-2);
  lua_remove(l,-2);
}

//END GenericWrapperByValue-----------------------------------------------------------


template<typename T>
class Comparable {
public:
  static void register_in_index_table(lua_State* l);
  static int eq_event(lua_State* l);
};

template<typename T>
void Comparable<T>::register_in_index_table(lua_State* l) {
  Wrapper<T>::set_meta_method(l, &eq_event, "__eq");
}

template<typename T>
int Comparable<T>::eq_event(lua_State* l) {
  int n = lua_gettop(l);
  if(n != 2)
    luaL_error(l, "Wrong argument count %d for __eq", n);
  T *p1 = Wrapper<T>::retrieve(l, 1, AssertOk);
  T *p2 = Wrapper<T>::retrieve(l, 2, AssertOk);
  lua_pop(l, n);
  lua_pushboolean(l, *p1 == *p2);
  return 1;
}

template<typename T>
class Summable {
public:
  static void register_in_index_table(lua_State* l);
  static int add_event(lua_State* l);
  static int sub_event(lua_State* l);
  static int unm_event(lua_State* l);
};

template<typename T>
void Summable<T>::register_in_index_table(lua_State* l) {
  Wrapper<T>::set_meta_method(l, &add_event, "__add");
  Wrapper<T>::set_meta_method(l, &sub_event, "__sub");
  Wrapper<T>::set_meta_method(l, &unm_event, "__unm");
}

template<typename T>
int Summable<T>::add_event(lua_State* l) {
  int n = lua_gettop(l);
  if(n != 2)
    luaL_error(l, "Wrong argument count %d for __add", n);
  T *p1 = Wrapper<T>::retrieve(l, 1, AssertOk);
  T *p2 = Wrapper<T>::retrieve(l, 2, AssertOk);
  lua_pop(l, n);

  Wrapper<T>::allocate(l, new T(*p1 + *p2));

  return 1;
}

template<typename T>
int Summable<T>::sub_event(lua_State* l) {
  int n = lua_gettop(l);
  if(n != 2)
    luaL_error(l, "Wrong argument count %d for __sub", n);
  T *p1 = Wrapper<T>::retrieve(l, 1, AssertOk);
  T *p2 = Wrapper<T>::retrieve(l, 2, AssertOk);
  lua_pop(l, n);

  Wrapper<T>::allocate(l, new T(*p1 - *p2));

  return 1;
}

template<typename T>
int Summable<T>::unm_event(lua_State* l) {
  int n = lua_gettop(l);

  /* for some strange reason unm gets 2 copies of the same argument */
  if(n != 2)
    luaL_error(l, "Wrong argument count %d for __unm", n);
  T *p1 = Wrapper<T>::retrieve(l, 1, AssertOk);
  lua_pop(l, n);

  Wrapper<T>::allocate(l, new T(-*p1));

  return 1;
}

} //end namespace LuaApi


#endif //LUAAPI__GENERICWRAPPER_H
