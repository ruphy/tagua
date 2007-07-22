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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RO_CLASS(sub_options, get()->subOptions, OptList)
  PROPERTY_RW(value, get()->value, get()->setValue, boolean)

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RO_TYPE(min, get()->min, number, int)
  PROPERTY_RO_TYPE(max, get()->max, number, int)
  PROPERTY_RW_TYPE(value, get()->value, get()->setValue, number, int)
  static int get_visualization(lua_State* l) {
    IntOpt::Visualization vis = retrieve(l, 1, DontCheck)->get()->visualization();
    switch (vis) {
    case IntOpt::SpinBox:
      lua_pushstring(l, "spinbox");
      break;
    case IntOpt::Slider:
      lua_pushstring(l, "slider");
      break;
    }
    return 1;
  }  

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RW_QSTRING(value, get()->value, get()->setValue)

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RW_QSTRING(value, get()->value, get()->setValue)

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RW_CLASS(value, get()->value, get()->setValue, QColor)

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RW_CLASS(value, get()->value, get()->setValue, QFont)

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RW_TYPE(selected, get()->selected, get()->setSelected, number, int)

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
  PROPERTY_RO_QSTRING(name, get()->name)
  PROPERTY_RO_QSTRING(label, get()->label)
  PROPERTY_RO_CLASS(options, get()->options, BoolOptList)
  PROPERTY_RW_TYPE(selected, get()->selected, get()->setSelected, number, int)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<SelectOptPtr> ---------------------------------------------------------

} //end namespace luaapi

#endif //LUAAPI__OPTIONS_H
