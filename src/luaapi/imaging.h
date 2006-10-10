/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LUAAPI__IMAGING_H
#define LUAAPI__IMAGING_H

#include <QBrush>
#include "loader/image.h"
#include "luaapi/genericwrapper.h"

namespace LuaApi {

//BEGIN Wrapper<QRectF> -------------------------------------------------------

template <>
class Wrapper<QRectF> : public GenericWrapper<QRectF> {
public:
  PROPERTY_RW(x, x, setX, number)
  PROPERTY_RW(y, y, setY, number)
  PROPERTY_RW(width, width, setWidth, number)
  PROPERTY_RW(height, height, setHeight, number)
  PROPERTY_RO(left, left, number)
  PROPERTY_RO(right, right, number)
  PROPERTY_RO(top, top, number)
  PROPERTY_RO(bottom, bottom, number)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);

  static int translate(lua_State* l);
};

//END Wrapper<QRectF> ---------------------------------------------------------


//BEGIN Wrapper<QPointF> ------------------------------------------------------

template <>
class Wrapper<QPointF> : public GenericWrapper<QPointF> {
public:
  PROPERTY_RW(x, x, setX, number)
  PROPERTY_RW(y, y, setY, number)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
  static int get_length(lua_State* l);
};

//END Wrapper<QPointF> --------------------------------------------------------


//BEGIN Wrapper<QColor>--------------------------------------------------------

template <>
class Wrapper<QColor> : public GenericWrapper<QColor> {
public:
  PROPERTY_RW_TYPE(r, red, setRed, number, int)
  PROPERTY_RW_TYPE(g, green, setGreen, number, int)
  PROPERTY_RW_TYPE(b, blue, setBlue, number, int)
  PROPERTY_RW_TYPE(a, alpha, setAlpha, number, int)

  static const char* class_name();
  static int to_string(lua_State* l);
  static void create_index_table(lua_State* l);
  static QColor get(lua_State* l, int index);
  static int constructor(lua_State* l);
};

//END Wrapper<QColor>----------------------------------------------------------


//BEGIN GradientWrapper--------------------------------------------------------

template<typename T>
class GradientWrapper : public GenericWrapper<T> {
public:
  static int index_event(lua_State* l);
  static int newindex_event(lua_State* l);
};

//END GradientWrapper----------------------------------------------------------


//BEGIN Wrapper<QLinearGradient>-----------------------------------------------

template <>
class Wrapper<QLinearGradient> : public GradientWrapper<QLinearGradient> {
public:
#if QT_VERSION >= 0x040200
  PROPERTY_RW_CLASS(start, start, setStart, QPointF)
  PROPERTY_RW_CLASS(finish, finalStop, setFinalStop, QPointF)
#endif //QT_VERSION >= 0x040200

  static const char* class_name();
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<QLinearGradient>-------------------------------------------------


//BEGIN Wrapper<QRadialGradient>-----------------------------------------------

template <>
class Wrapper<QRadialGradient> : public GradientWrapper<QRadialGradient> {
public:
#if QT_VERSION >= 0x040200
  PROPERTY_RW_CLASS(center, center, setCenter, QPointF)
  PROPERTY_RW(radius, radius, setRadius, number)
  PROPERTY_RW_CLASS(focus, focalPoint, setFocalPoint, QPointF)
#endif //QT_VERSION >= 0x040200

  static const char* class_name();
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<QRadialGradient>-------------------------------------------------


//BEGIN Wrapper<QConicalGradient>----------------------------------------------

template <>
class Wrapper<QConicalGradient> : public GradientWrapper<QConicalGradient> {
public:
#if QT_VERSION >= 0x040200
  PROPERTY_RW_CLASS(center, center, setCenter, QPointF)
  PROPERTY_RW(angle, angle, setAngle, number)
#endif //QT_VERSION >= 0x040200

  static const char* class_name();
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<QConicalGradient>------------------------------------------------


//BEGIN Wrapper<QBrush>--------------------------------------------------------

template <>
class Wrapper<QBrush> : public GenericWrapper<QBrush> {
public:
  static const char* class_name();
  static void create_index_table(lua_State* l);
  static QBrush get(lua_State* l, int index);
  static int constructor(lua_State* l);

  static int rotate(lua_State* l);
  static int scale(lua_State* l);
  static int translate(lua_State* l);
};

//END Wrapper<QBrush>----------------------------------------------------------


//BEGIN Wrapper<Image> --------------------------------------------------------

template<>
class Wrapper<Loader::Image> : public GenericWrapper<Loader::Image> {
public:
  PROPERTY_RO(width, width, number)
  PROPERTY_RO(height, height, number)
  PROPERTY_RW(opacity, opacity, setOpacity, number)

  static const char* class_name();
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);

  static int resetMatrix(lua_State* l);
  static int rotate(lua_State* l);
  static int scale(lua_State* l);
  static int translate(lua_State* l);
  static int clear(lua_State* l);
  static int setPaintOver(lua_State* l);
  static int fillRect(lua_State* l);
  static int drawLine(lua_State* l);
  static int drawImage(lua_State* l);
  static int drawSVG(lua_State* l);
  static int drawGlyph(lua_State* l);
  static int createShadow(lua_State* l);
};

//END Wrapper<Image> ----------------------------------------------------------

//BEGIN Wrapper<Glyph> --------------------------------------------------------

template<>
class Wrapper<Loader::Glyph> : public GenericWrapper<Loader::Glyph> {
public:
  static const char* class_name();
  static void create_index_table(lua_State* l);
  static int constructor(lua_State* l);
};

//END Wrapper<Glyph> ----------------------------------------------------------

} //end namespace LuaApi


#endif //LUAAPI__IMAGING_H
