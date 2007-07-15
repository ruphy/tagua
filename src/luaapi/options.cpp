/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "options.h"
#include "luaapi/loader.h"

namespace LuaApi {

//BEGIN Wrapper<OptList> -------------------------------------------------------

const char* Wrapper<OptList>::class_name() {
  return "OptList";
}

int Wrapper<OptList>::to_string(lua_State* l) {
  OptList r = *retrieve(l, 1, AssertOk);
  QString s;
  for(int i=0;i<r.size();i++)
    s += (i==0?"":",")+r[i]->name();
  s = "list["+s+"]";
  lua_pop(l, 1);
  lua_pushstring(l, s.toAscii().constData());
  return 1;
}

int Wrapper<OptList>::newindex_event(lua_State* l) {
  lua_pop(l,3);
  luaL_error(l, "OptList cannot be modified after being constructed!!!");
  return 0;
}

int Wrapper<OptList>::index_event(lua_State* l) {
  const char *k = lua_tostring(l, -1);
  OptList& list = *retrieve(l, -2);
  lua_pop(l,2);
  for(int i=0; i<list.size();i++) {
    if(list[i]->name() == k) {
      if(BoolOptPtr p = boost::dynamic_pointer_cast<BoolOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<BoolOptPtr>::create(l, p);
      else if(IntOptPtr p = boost::dynamic_pointer_cast<IntOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<IntOptPtr>::create(l, p);
      else if(StringOptPtr p = boost::dynamic_pointer_cast<StringOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<StringOptPtr>::create(l, p);
      else if(UrlOptPtr p = boost::dynamic_pointer_cast<UrlOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<UrlOptPtr>::create(l, p);
      else if(ColorOptPtr p = boost::dynamic_pointer_cast<ColorOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<ColorOptPtr>::create(l, p);
      else if(FontOptPtr p = boost::dynamic_pointer_cast<FontOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<FontOptPtr>::create(l, p);
      else if(ComboOptPtr p = boost::dynamic_pointer_cast<ComboOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<ComboOptPtr>::create(l, p);
      else if(SelectOptPtr p = boost::dynamic_pointer_cast<SelectOpt, BaseOpt>(list[i]))
        GenericWrapperByValue<SelectOptPtr>::create(l, p);
      else
        luaL_error(l, "WFT?");
      return 1;
    }
  }
  luaL_error(l, "No item with name %s", k);
  return 0;
}

void Wrapper<OptList>::create_index_table(lua_State* l) {
  set_meta_method(l, &to_string, "__tostring");
  set_meta_method(l, &newindex_event, "__newindex");
  set_meta_method(l, &index_event, "__index");

  Comparable<OptList>::register_in_index_table(l);
}

int Wrapper<OptList>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=1)
    luaL_error(l, "Wrong parameter list for OptList constructor");

  OptList list;
  lua_pushnil(l);
  while (lua_next(l, -2) != 0) {
    if(BoolOptPtr *p = Wrapper<BoolOptPtr>::retrieve(l, -1))
      list << BoolOptPtr(*p);
    else if(IntOptPtr *p = Wrapper<IntOptPtr>::retrieve(l, -1))
      list << IntOptPtr(*p);
    else if(StringOptPtr *p = Wrapper<StringOptPtr>::retrieve(l, -1))
      list << StringOptPtr(*p);
    else if(UrlOptPtr *p = Wrapper<UrlOptPtr>::retrieve(l, -1))
      list << UrlOptPtr(*p);
    else if(ColorOptPtr *p = Wrapper<ColorOptPtr>::retrieve(l, -1))
      list << ColorOptPtr(*p);
    else if(FontOptPtr *p = Wrapper<FontOptPtr>::retrieve(l, -1))
      list << FontOptPtr(*p);
    else if(ComboOptPtr *p = Wrapper<ComboOptPtr>::retrieve(l, -1))
      list << ComboOptPtr(*p);
    else if(SelectOptPtr *p = Wrapper<SelectOptPtr>::retrieve(l, -1))
      list << SelectOptPtr(*p);
    else
      luaL_error(l, "Wrong item in list for OptList constructor");
    lua_pop(l, 1);
  }
  lua_pop(l, n);

//   for(int i=0;i<list.size();i++)
//     std::cout << "list[" << i << "] = " << list[i]->name() << std::endl;
  create(l, list);
  return 1;
}

//END Wrapper<OptList> ---------------------------------------------------------


//BEGIN Wrapper<BoolOptList> -------------------------------------------------------

const char* Wrapper<BoolOptList>::class_name() {
  return "BoolOptList";
}

int Wrapper<BoolOptList>::to_string(lua_State* l) {
  BoolOptList r = *retrieve(l, 1, AssertOk);
  QString s;
  for(int i=0;i<r.size();i++)
    s += (i==0?"":",")+r[i]->name();
  s = "bool_list["+s+"]";
  lua_pop(l, 1);
  lua_pushstring(l, s.toAscii().constData());
  return 1;
}

int Wrapper<BoolOptList>::newindex_event(lua_State* l) {
  lua_pop(l,3);
  luaL_error(l, "BoolOptList cannot be modified after being constructed!!!");
  return 0;
}

int Wrapper<BoolOptList>::index_event(lua_State* l) {
  const char *k = lua_tostring(l, -1);
  BoolOptList& list = *retrieve(l, -2);
  lua_pop(l,2);
  for(int i=0; i<list.size();i++) {
    if(list[i]->name() == k) {
      GenericWrapperByValue<BoolOptPtr>::create(l, list[i]);
      return 1;
    }
  }
  luaL_error(l, "No bool item with name %s", k);
  return 0;
}

void Wrapper<BoolOptList>::create_index_table(lua_State* l) {
  set_meta_method(l, &to_string, "__tostring");
  set_meta_method(l, &newindex_event, "__newindex");
  set_meta_method(l, &index_event, "__index");

  Comparable<BoolOptList>::register_in_index_table(l);
}

int Wrapper<BoolOptList>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=1)
    luaL_error(l, "Wrong parameter list for BoolOptList constructor");

  BoolOptList list;
  lua_pushnil(l);
  while (lua_next(l, -2) != 0) {
    list << *Wrapper<BoolOptPtr>::retrieve(l, -1, AssertOk);
    lua_pop(l, 1);
  }
  lua_pop(l, n);

  create(l, list);
  return 1;
}

//END Wrapper<BoolOptList> ---------------------------------------------------------


//BEGIN Wrapper<BoolOptPtr> -------------------------------------------------------

const char* Wrapper<BoolOptPtr>::class_name() {
  return "BoolOpt";
}

int Wrapper<BoolOptPtr>::to_string(lua_State* l) {
  BoolOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "bool[%s] = %s", r->name().toAscii().constData(), r->value()?"true":"false" );
  return 1;
}

void Wrapper<BoolOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RO(l, sub_options);
  SET_PROPERTY_RW(l, value);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<BoolOptPtr>::register_in_index_table(l);
}

int Wrapper<BoolOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n<3 || n>4)
    luaL_error(l, "Wrong parameter list for BoolOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  bool value = lua_toboolean(l, 3);
  OptList sub_opt;
  if(n==4)
    sub_opt = *Wrapper<OptList>::retrieve(l, 4, AssertOk);

  lua_pop(l, n);
  create(l, BoolOptPtr(new BoolOpt(name, label, value, sub_opt)));
  return 1;
}

//END Wrapper<BoolOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<IntOptPtr> -------------------------------------------------------

const char* Wrapper<IntOptPtr>::class_name() {
  return "IntOpt";
}

int Wrapper<IntOptPtr>::to_string(lua_State* l) {
  IntOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "int[%s] = %d", r->name().toAscii().constData(), r->value() );
  return 1;
}

void Wrapper<IntOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RO(l, min);
  SET_PROPERTY_RO(l, max);
  SET_PROPERTY_RW(l, value);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<IntOptPtr>::register_in_index_table(l);
}

int Wrapper<IntOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=5)
    luaL_error(l, "Wrong parameter list for IntOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  int value = int(lua_tonumber(l, 3));
  int min = int(lua_tonumber(l, 4));
  int max = int(lua_tonumber(l, 5));

  lua_pop(l, n);
  create(l, IntOptPtr(new IntOpt(name, label, value, min, max)));
  return 1;
}

//END Wrapper<IntOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<StringOptPtr> -------------------------------------------------------

const char* Wrapper<StringOptPtr>::class_name() {
  return "StringOpt";
}

int Wrapper<StringOptPtr>::to_string(lua_State* l) {
  StringOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "string[%s] = %s", r->name().toAscii().constData(), r->value().toAscii().constData() );
  return 1;
}

void Wrapper<StringOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RW(l, value);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<StringOptPtr>::register_in_index_table(l);
}

int Wrapper<StringOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=3)
    luaL_error(l, "Wrong parameter list for StringOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  const char* value = lua_tostring(l, 3);
  lua_pop(l, n);

  create(l, StringOptPtr(new StringOpt(name, label, value)));
  return 1;
}

//END Wrapper<StringOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<UrlOptPtr> -------------------------------------------------------

const char* Wrapper<UrlOptPtr>::class_name() {
  return "UrlOpt";
}

int Wrapper<UrlOptPtr>::to_string(lua_State* l) {
  UrlOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "url[%s] = %s", r->name().toAscii().constData(), r->value().toAscii().constData() );
  return 1;
}

void Wrapper<UrlOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RW(l, value);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<UrlOptPtr>::register_in_index_table(l);
}

int Wrapper<UrlOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=3)
    luaL_error(l, "Wrong parameter list for UrlOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  const char* value = lua_tostring(l, 3);
  lua_pop(l, n);

  // retrieve the api and current base directory for the url
  lua_getfield(l, LUA_REGISTRYINDEX, API_LOADER);
  LuaApi::Loader* api = reinterpret_cast<Loader*>(lua_touserdata(l, -1));
  lua_pop(l, 1);

  create(l, UrlOptPtr(new UrlOpt(name, label, api->currDir().absoluteFilePath(value) )));
  return 1;
}

//END Wrapper<UrlOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<ColorOptPtr> -------------------------------------------------------

const char* Wrapper<ColorOptPtr>::class_name() {
  return "ColorOpt";
}

int Wrapper<ColorOptPtr>::to_string(lua_State* l) {
  ColorOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "color[%s] = %s", r->name().toAscii().constData(), r->value().name().toAscii().constData() );
  return 1;
}

void Wrapper<ColorOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RW(l, value);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<ColorOptPtr>::register_in_index_table(l);
}

int Wrapper<ColorOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=3)
    luaL_error(l, "Wrong parameter list for ColorOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  const char* value = lua_tostring(l, 3);
  lua_pop(l, n);

  create(l, ColorOptPtr(new ColorOpt(name, label, value)));
  return 1;
}

//END Wrapper<ColorOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<FontOptPtr> -------------------------------------------------------

const char* Wrapper<FontOptPtr>::class_name() {
  return "FontOpt";
}

int Wrapper<FontOptPtr>::to_string(lua_State* l) {
  FontOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "font[%s] = %s", r->name().toAscii().constData(), r->value().toString().toAscii().constData() );
  return 1;
}

void Wrapper<FontOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  //SET_PROPERTY_RW(l, value);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<FontOptPtr>::register_in_index_table(l);
}

int Wrapper<FontOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n!=3)
    luaL_error(l, "Wrong parameter list for FontOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  const char* value = lua_tostring(l, 3);
  lua_pop(l, n);

  create(l, FontOptPtr(new FontOpt(name, label, QFont(value))));
  return 1;
}

//END Wrapper<FontOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<ComboOptPtr> -------------------------------------------------------

const char* Wrapper<ComboOptPtr>::class_name() {
  return "ComboOpt";
}

int Wrapper<ComboOptPtr>::to_string(lua_State* l) {
  ComboOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "combo[%s] = %s", r->name().toAscii().constData(), r->m_values.join(",").toAscii().constData() );
  return 1;
}

void Wrapper<ComboOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RW(l, selected);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<ComboOptPtr>::register_in_index_table(l);
}

int Wrapper<ComboOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n<3 || n>4)
    luaL_error(l, "Wrong parameter list for ComboOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  int selected = n==4 ? int(lua_tonumber(l, 4)) : 0;
  QStringList values;
  lua_pushnil(l);
  while (lua_next(l, 3) != 0) {
    values << lua_tostring(l, -1);
    lua_pop(l, 1);
  }

  lua_pop(l, n);
  create(l, ComboOptPtr(new ComboOpt(name, label, values, selected)));
  return 1;
}

//END Wrapper<ComboOptPtr> ---------------------------------------------------------


//BEGIN Wrapper<SelectOptPtr> -------------------------------------------------------

const char* Wrapper<SelectOptPtr>::class_name() {
  return "SelectOpt";
}

int Wrapper<SelectOptPtr>::to_string(lua_State* l) {
  SelectOptPtr r = *retrieve(l, 1, AssertOk);
  lua_pop(l, 1);
  lua_pushfstring(l, "select[%s] = !!", r->name().toAscii().constData() );
  return 1;
}

void Wrapper<SelectOptPtr>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, name);
  SET_PROPERTY_RO(l, label);
  SET_PROPERTY_RO(l, options);
  SET_PROPERTY_RW(l, selected);
  set_meta_method(l, &to_string, "__tostring");

  Comparable<SelectOptPtr>::register_in_index_table(l);
}

int Wrapper<SelectOptPtr>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  if(n<3 || n>4)
    luaL_error(l, "Wrong parameter list for SelectOpt constructor");

  const char* name = lua_tostring(l, 1);
  const char* label = lua_tostring(l, 2);
  BoolOptList values = *Wrapper<BoolOptList>::retrieve(l, 3, AssertOk);
  int selected = n==4 ? int(lua_tonumber(l, 4)) : 0;

  lua_pop(l, n);
  create(l, SelectOptPtr(new SelectOpt(name, label, values, selected)));
  return 1;
}

//END Wrapper<SelectOptPtr> ---------------------------------------------------------


} //end namespace luaapi

