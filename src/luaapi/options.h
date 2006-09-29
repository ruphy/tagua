/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LUAAPI__OPTIONS_H
#define LUAAPI__OPTIONS_H

#include "option.h"
#include "luaapi/genericwrapper.h"
#include "luaapi/imaging.h"

namespace LuaApi {

//BEGIN Wrapper<OptList> -------------------------------------------------------

template <>
class Wrapper<OptList> : public GenericWrapperByValue<OptList> {
public:
  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
  static int newindex_event(lua_State* l);
  static int index_event(lua_State* l);
};

//END Wrapper<OptList> ---------------------------------------------------------

//BEGIN Wrapper<BoolOptList> -------------------------------------------------------

template <>
class Wrapper<BoolOptList> : public GenericWrapperByValue<BoolOptList> {
public:
  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
  static int newindex_event(lua_State* l);
  static int index_event(lua_State* l);
};

//END Wrapper<BoolOptList> ---------------------------------------------------------

//BEGIN Wrapper<BoolOptPtr> -------------------------------------------------------

template <>
class Wrapper<BoolOptPtr> : public GenericWrapperByValue<BoolOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RO_P_CLASS(sub_options, subOptions, OptList)
  P_PROPERTY_RW(value, value, setValue, boolean)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<BoolOptPtr> ---------------------------------------------------------

//BEGIN Wrapper<IntOptPtr> -------------------------------------------------------

template <>
class Wrapper<IntOptPtr> : public GenericWrapperByValue<IntOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RO_TYPE(min, min, number, int)
  P_PROPERTY_RO_TYPE(max, max, number, int)
  P_PROPERTY_RW_TYPE(value, value, setValue, number, int)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<IntOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<StringOptPtr> -------------------------------------------------------

template <>
class Wrapper<StringOptPtr> : public GenericWrapperByValue<StringOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RW_QSTRING(value, value, setValue)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<StringOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<UrlOptPtr> -------------------------------------------------------

template <>
class Wrapper<UrlOptPtr> : public GenericWrapperByValue<UrlOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RW_QSTRING(value, value, setValue)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<UrlOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<ColorOptPtr> -------------------------------------------------------

template <>
class Wrapper<ColorOptPtr> : public GenericWrapperByValue<ColorOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RW_CLASS(value, value, setValue, QColor)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<ColorOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<FontOptPtr> -------------------------------------------------------

template <>
class Wrapper<FontOptPtr> : public GenericWrapperByValue<FontOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  //P_PROPERTY_RW_QSTRING(value, value, setValue)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<FontOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<ComboOptPtr> -------------------------------------------------------

template <>
class Wrapper<ComboOptPtr> : public GenericWrapperByValue<ComboOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RW_TYPE(selected, selected, setSelected, number, int)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<ComboOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<SelectOptPtr> -------------------------------------------------------

template <>
class Wrapper<SelectOptPtr> : public GenericWrapperByValue<SelectOptPtr> {
public:
  P_PROPERTY_RO_QSTRING(name, name)
  P_PROPERTY_RO_QSTRING(label, label)
  P_PROPERTY_RO_P_CLASS(options, options, BoolOptList)
  P_PROPERTY_RW_TYPE(selected, selected, setSelected, number, int)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<SelectOptPtr> ---------------------------------------------------------

} //end namespace luaapi

#endif //LUAAPI__OPTIONS_H
