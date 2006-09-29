/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <cmath>
#include "luaapi/imaging.h"

#if QT_VERSION >= 0x040200
  #define DEFAULT_GRADIENT
#endif

using namespace Loader;

namespace LuaApi {

//BEGIN Wrapper<QRectF> -------------------------------------------------------

const char* Wrapper<QRectF>::class_name() {
  return "Rect";
}

int Wrapper<QRectF>::translate(lua_State* l) {
  QRectF *r = retrieve(l, 1, AssertOk);
  QPointF *p = Wrapper<QPointF>::retrieve(l, 2, AssertOk);
  r->translate(*p);
  return 0;
}

int Wrapper<QRectF>::to_string(lua_State* l) {
  QRectF *r = retrieve(l, 1, AssertOk);
  lua_pushfstring(l, "(%f, %f, %f, %f)", r->x(), r->y(), r->width(), r->height() );
  return 1;
}

void Wrapper<QRectF>::create_index_table(lua_State* l) {
  SET_PROPERTY_RW(l, x);
  SET_PROPERTY_RW(l, y);
  SET_PROPERTY_RW(l, width);
  SET_PROPERTY_RW(l, height);

  SET_PROPERTY_RO(l, left);
  SET_PROPERTY_RO(l, right);
  SET_PROPERTY_RO(l, top);
  SET_PROPERTY_RO(l, bottom);

  set_method(l, &translate, "translate");
  set_meta_method(l, &to_string, "__tostring");

  Comparable<QRectF>::register_in_index_table(l);
}

int Wrapper<QRectF>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  QRectF* res;
  switch (n) {
  case 0:
    res = new QRectF;
    break;
  case 1:
    res = new QRectF(*retrieve(l, 1, AssertOk));
    break;
  case 4:
    {
      double x = lua_tonumber(l, 1);
      double y = lua_tonumber(l, 2);
      double width = lua_tonumber(l, 3);
      double height = lua_tonumber(l, 4);

      res = new QRectF(x, y, width, height);
    }
    break;
  default:
    res = 0;
    luaL_error(l, "Wrong parameter list for Rect constructor");
  }
  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

//END Wrapper<QRectF> ---------------------------------------------------------


//BEGIN Wrapper<QPointF> ------------------------------------------------------

const char* Wrapper<QPointF>::class_name() {
  return "Point";
}

int Wrapper<QPointF>::to_string(lua_State* l) {
  QPointF *p = retrieve(l, 1, AssertOk);
  lua_pushfstring(l, "(%f, %f)", p->x(), p->y() );
  return 1;
}

void Wrapper<QPointF>::create_index_table(lua_State* l) {
  SET_PROPERTY_RW(l, x);
  SET_PROPERTY_RW(l, y);
  SET_PROPERTY_RO(l, length);

  set_meta_method(l, &to_string, "__tostring");
  set_meta_method(l, &get_length, "__len");

  Comparable<QPointF>::register_in_index_table(l);
  Summable<QPointF>::register_in_index_table(l);
}

int Wrapper<QPointF>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  QPointF* res;
  switch(n) {
  case 1:
    res = new QPointF(*retrieve(l, 1, AssertOk));
    break;
  case 2: {
    double x = lua_tonumber(l, 1);
    double y = lua_tonumber(l, 2);
    res = new QPointF(x, y);
    }
    break;
  default:
    res = 0;
    luaL_error(l, "Wrong argument count for Point constructor");
    break;
  }

  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

int Wrapper<QPointF>::get_length(lua_State* l) {
  QPointF *p = retrieve(l, 1, DontCheck);
  lua_pushnumber(l, sqrt(p->x()*p->x() + p->y()*p->y()) );
  return 1;
}

//END Wrapper<QPointF> --------------------------------------------------------


//BEGIN Wrapper<QColor>--------------------------------------------------------


const char* Wrapper<QColor>::class_name() {
  return "Color";
}

int Wrapper<QColor>::to_string(lua_State* l) {
  QColor *r = retrieve(l, 1, AssertOk);
  char buf[32];
  snprintf(buf, 32, "#%08x", r->rgba());
  lua_pushstring(l, buf);
  return 1;
}

void Wrapper<QColor>::create_index_table(lua_State* l) {
  SET_PROPERTY_RW(l, r);
  SET_PROPERTY_RW(l, g);
  SET_PROPERTY_RW(l, b);
  SET_PROPERTY_RW(l, a);

  set_meta_method(l, &to_string, "__tostring");
}

QColor Wrapper<QColor>::get(lua_State* l, int index) {
  if(lua_isstring(l, index)) {
    QColor retv(lua_tostring(l, index));
    if(!retv.isValid())
      luaL_error(l, "Wrong color string \"%s\"", lua_tostring(l, index));
    return retv;
  }
  else
    return *retrieve(l, index, AssertOk);
}

int Wrapper<QColor>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  QColor* res;
  switch (n) {
  case 0:
    res = new QColor();
    break;
  case 1:
    res = new QColor(get(l, 1));
    break;
  case 3:
  case 4:
    {
      int r = int(lua_tonumber(l, 1));
      int g = int(lua_tonumber(l, 2));
      int b = int(lua_tonumber(l, 3));
      int a = (n == 3) ? 255 : int(lua_tonumber(l, 4));

      res = new QColor(r, g, b, a);
    }
    break;
  default:
    res = 0;
    luaL_error(l, "Wrong parameter list for Color constructor");
  }
  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

//END Wrapper<QColor>----------------------------------------------------------

template<typename T>
int GradientWrapper<T>::index_event(lua_State* l) {
  if(!lua_isnumber(l, -1))
    return GenericWrapperBase::object_meta_index_event(l);

  T *g = Wrapper<T>::retrieve(l, -2, AssertOk);
  double stopnum = lua_tonumber(l, -1);
  lua_pop(l, 1);

  QGradientStops s = g->stops();
  QColor retv;
  if(s.size()) {
    int min = 0;
    int max = s.size();
    while(min < max-1) {
      int mid = (min+max)/2;
      if(stopnum > s[(min+max)/2].first)
        min = mid;
      else
        max = mid;
    }
    if(qAbs(s[max].first-stopnum) < qAbs(s[min].first-stopnum)) {
      stopnum = s[max].first;
      retv = s[max].second;
    }
    else {
      stopnum = s[min].first;
      retv = s[min].second;
    }
  }
  Wrapper<QColor>::allocate(l, new QColor(retv));
  lua_pushnumber(l, stopnum);

  return 2;
}

template<typename T>
int GradientWrapper<T>::newindex_event(lua_State* l) {
  if(!lua_isnumber(l, -2))
    return GenericWrapperBase::object_meta_newindex_event(l);

  T *g = Wrapper<T>::retrieve(l, -3, AssertOk);
  double stopnum = lua_tonumber(l, -2);
  QColor col = Wrapper<QColor>::get(l, -1);
  lua_pop(l, 3);

  g->setColorAt(stopnum, col);
  return 0;
}


//BEGIN Wrapper<QLinearGradient>--------------------------------------------------------

const char* Wrapper<QLinearGradient>::class_name() {
  return "LinearGradient";
}

void Wrapper<QLinearGradient>::create_index_table(lua_State* l) {
#if QT_VERSION >= 0x040200
  SET_PROPERTY_RW(l, start);
  SET_PROPERTY_RW(l, finish);
#endif //QT_VERSION >= 0x040200

  //set_meta_method(l, &index_event, "__index");
  set_meta_method(l, &newindex_event, "__newindex");
}

int Wrapper<QLinearGradient>::constructor(lua_State* l) {
  const int n = lua_gettop(l);
  QLinearGradient* res;

  switch(n) {
#ifdef DEFAULT_GRADIENT
    case 0:
      res =  new QLinearGradient();
      break;
#endif // DEFAULT_GRADIENT
    case 2: {
      QPointF* start = Wrapper<QPointF>::retrieve(l, 1, AssertOk);
      QPointF* final = Wrapper<QPointF>::retrieve(l, 2, AssertOk);
      res =  new QLinearGradient(*start, *final);
      break;
    }
    default:
      res = 0;
      luaL_error(l, "Wrong parameter list for LinearGradient constructor");
      break;
  }

  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

//END Wrapper<QLinearGradient>----------------------------------------------------------


//BEGIN Wrapper<QRadialGradient>--------------------------------------------------------

const char* Wrapper<QRadialGradient>::class_name() {
  return "RadialGradient";
}

void Wrapper<QRadialGradient>::create_index_table(lua_State* l) {
#if QT_VERSION >= 0x040200
  SET_PROPERTY_RW(l, center);
  SET_PROPERTY_RW(l, radius);
  SET_PROPERTY_RW(l, focus);
#endif //QT_VERSION >= 0x040200

  //set_meta_method(l, &index_event, "__index");
  set_meta_method(l, &newindex_event, "__newindex");
}

int Wrapper<QRadialGradient>::constructor(lua_State* l) {
  const int n = lua_gettop(l);
  QRadialGradient* res;

  switch(n) {
#ifdef DEFAULT_GRADIENT
    case 0:
      res =  new QRadialGradient();
      break;
#endif // DEFAULT_GRADIENT
    case 2: {
      QPointF* center = Wrapper<QPointF>::retrieve(l, 1, AssertOk);
      double radius = lua_tonumber(l, 2);
      res =  new QRadialGradient(*center, radius);
      break;
    }
    case 3: {
      QPointF* center = Wrapper<QPointF>::retrieve(l, 1, AssertOk);
      double radius = lua_tonumber(l, 2);
      QPointF* focus = Wrapper<QPointF>::retrieve(l, 3, AssertOk);
      res =  new QRadialGradient(*center, radius, *focus);
      break;
    }
    default:
      res = 0;
      luaL_error(l, "Wrong parameter list for RadialGradient constructor");
      break;
  }

  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

//END Wrapper<QRadialGradient>----------------------------------------------------------

//BEGIN Wrapper<QConicalGradient>--------------------------------------------------------

const char* Wrapper<QConicalGradient>::class_name() {
  return "ConicalGradient";
}

void Wrapper<QConicalGradient>::create_index_table(lua_State* l) {
#if QT_VERSION >= 0x040200
  SET_PROPERTY_RW(l, center);
  SET_PROPERTY_RW(l, angle);
#endif //QT_VERSION >= 0x040200

  //set_meta_method(l, &index_event, "__index");
  set_meta_method(l, &newindex_event, "__newindex");
}

int Wrapper<QConicalGradient>::constructor(lua_State* l) {
  const int n = lua_gettop(l);
  QConicalGradient* res;

  switch(n) {
#ifdef DEFAULT_GRADIENT
    case 0:
      res =  new QConicalGradient();
      break;
#endif // DEFAULT_GRADIENT
    case 2: {
      QPointF* center = Wrapper<QPointF>::retrieve(l, 1, AssertOk);
      double angle = lua_tonumber(l, 2);
      res =  new QConicalGradient(*center, angle);
      break;
    }
    default:
      res = 0;
      luaL_error(l, "Wrong parameter list for Color constructor");
      break;
  }

  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

//END Wrapper<QConicalGradient>----------------------------------------------------------

//BEGIN Wrapper<QBrush>--------------------------------------------------------

const char* Wrapper<QBrush>::class_name() {
  return "Brush";
}

void Wrapper<QBrush>::create_index_table(lua_State* l) {

  //set_meta_method(l, &to_string, "__tostring");
  set_method(l, &translate, "translate");
  set_method(l, &rotate, "rotate");
  set_method(l, &scale, "scale");
}

QBrush Wrapper<QBrush>::get(lua_State* l, int index) {
  if(QBrush *b = retrieve(l, index))
    return *b;
  else if(QLinearGradient* g = Wrapper<QLinearGradient>::retrieve(l, index))
    return QBrush(*g);
  else if(QRadialGradient* g = Wrapper<QRadialGradient>::retrieve(l, index))
    return QBrush(*g);
  else if(QConicalGradient* g = Wrapper<QConicalGradient>::retrieve(l, index))
    return QBrush(*g);
  else if(Loader::Image* img = Wrapper<Image>::retrieve(l, index)) {
#if QT_VERSION >= 0x040200
    return QBrush(img->m_image);
#else
    Q_UNUSED(img);
    luaL_error(l, "Sorry my dear, you cannot create brushes from images with Qt < 4.2");
    return QBrush();
#endif
  }
  else
    return QBrush(Wrapper<QColor>::get(l, index));
}

int Wrapper<QBrush>::constructor(lua_State* l) {
  const int n = lua_gettop(l);

  QBrush* res;
  switch (n) {
  case 0:
    res = new QBrush(Qt::NoBrush);
    break;
  case 1:
    res = new QBrush(get(l, 1));
    break;
  default:
    res = 0;
    luaL_error(l, "Wrong parameter list for Brush constructor");
  }
  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

#if QT_VERSION >= 0x040200

int Wrapper<QBrush>::rotate(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 2) luaL_error(l, "Wrong argument count for Brush::rotate");

  QBrush* brush = retrieve(l, 1, AssertOk);
  double r = lua_tonumber(l, 2);
  lua_pop(l, n);

  QMatrix m = brush->matrix();
  m.rotate(r);
  brush->setMatrix(m);
  return 0;
}

int Wrapper<QBrush>::scale(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 3) luaL_error(l, "Wrong argument count for Brush::scale");

  QBrush* brush = retrieve(l, 1, AssertOk);
  double x = lua_tonumber(l, 2);
  double y = lua_tonumber(l, 3);
  lua_pop(l, n);

  QMatrix m = brush->matrix();
  m.scale(x, y);
  brush->setMatrix(m);
  return 0;
}

int Wrapper<QBrush>::translate(lua_State* l) {
  const int n = lua_gettop(l);
  if (n < 2 || n > 3) luaL_error(l, "Wrong argument count for Brush::translate");

  QBrush* brush = retrieve(l, 1, AssertOk);
  if(n == 2) {
    double x = lua_tonumber(l, 2);
    double y = lua_tonumber(l, 3);
    lua_pop(l, n);

    QMatrix m = brush->matrix();
    m.translate(x, y);
    brush->setMatrix(m);
  }
  else {
    QPointF* point = Wrapper<QPointF>::retrieve(l, 2, AssertOk);
    lua_pop(l, n);

    QMatrix m = brush->matrix();
    m.translate(point->x(), point->y());
    brush->setMatrix(m);
  }
  return 0;
}
#else //QT_VERSION >= 0x040200

int Wrapper<QBrush>::rotate(lua_State* l) {
  luaL_error(l, "Sorry my dear, you cannot transform brushes with Qt < 4.2");
  return 0;
}

int Wrapper<QBrush>::scale(lua_State* l) {
  luaL_error(l, "Sorry my dear, you cannot transform brushes with Qt < 4.2");
  return 0;
}

int Wrapper<QBrush>::translate(lua_State* l) {
  luaL_error(l, "Sorry my dear, you cannot transform brushes with Qt < 4.2");
  return 0;
}
#endif //QT_VERSION >= 0x040200

//END Wrapper<QBrush>----------------------------------------------------------


//BEGIN Wrapper<Image> --------------------------------------------------------

const char* Wrapper<Image>::class_name() {
  return "Image";
}

void Wrapper<Image>::create_index_table(lua_State* l) {
  SET_PROPERTY_RO(l, width);
  SET_PROPERTY_RO(l, height);
  SET_PROPERTY_RW(l, opacity);

  set_method(l, &resetMatrix, "reset_matrix");
  set_method(l, &rotate, "rotate");
  set_method(l, &scale, "scale");
  set_method(l, &translate, "translate");
  set_method(l, &setPaintOver, "set_paint_over");
  set_method(l, &clear, "clear");
  set_method(l, &fillRect, "fill_rect");
  set_method(l, &drawLine, "draw_line");
  set_method(l, &drawImage, "draw_image");
  set_method(l, &drawSVG, "draw_svg");
  set_method(l, &drawGlyph, "draw_glyph");
  set_method(l, &createShadow, "create_shadow");
}

int Wrapper<Image>::constructor(lua_State* l) {
  const int n = lua_gettop(l);
  Image* res;
  switch (n) {
  case 1:
    {
      if(lua_isstring(l, 1)) {
        const char* file = lua_tostring(l, 1);
        Context* context = retrieve_context(l);
        res = new Image(context, file_path(l, file) );
      }
      else
        res = new Image(*retrieve(l, 1, AssertOk));
    }
    break;
  case 2:
    {
      int width = static_cast<int>(lua_tonumber(l, 1));
      int height = static_cast<int>(lua_tonumber(l, 2));
      res = new Image(width, height);
    }
    break;
  default:
    res = 0;
    luaL_error(l, "Wrong argument count for Image constructor");
  }
  lua_pop(l, n);

  allocate(l, res);
  return 1;
}

int Wrapper<Image>::resetMatrix(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 1) luaL_error(l, "Wrong argument count for Image::reset_matrix");

  Image* img = retrieve(l, 1, AssertOk);
  lua_pop(l, n);

  img->resetMatrix();
  return 0;
}

int Wrapper<Image>::rotate(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 2) luaL_error(l, "Wrong argument count for Image::rotate");

  Image* img = retrieve(l, 1, AssertOk);
  double r = lua_tonumber(l, 2);
  lua_pop(l, n);

  img->rotate(r);
  return 0;
}

int Wrapper<Image>::scale(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 3) luaL_error(l, "Wrong argument count for Image::scale");

  Image* img = retrieve(l, 1, AssertOk);
  double x = lua_tonumber(l, 2);
  double y = lua_tonumber(l, 3);
  lua_pop(l, n);

  img->scale(x, y);
  return 0;
}

int Wrapper<Image>::translate(lua_State* l) {
  const int n = lua_gettop(l);
  if (n < 2 || n > 3) luaL_error(l, "Wrong argument count for Image::translate");

  Image* img = retrieve(l, 1, AssertOk);
  if(n == 2) {
    double x = lua_tonumber(l, 2);
    double y = lua_tonumber(l, 3);
    lua_pop(l, n);

    img->translate(x, y);
  }
  else {
    QPointF* point = Wrapper<QPointF>::retrieve(l, 2, AssertOk);
    lua_pop(l, n);

    img->translate(point->x(), point->y());
  }
  return 0;
}

int Wrapper<Image>::clear(lua_State* l) {
  const int n = lua_gettop(l);
  if (n < 1 || n > 2) luaL_error(l, "Wrong argument count for Image::fill_rect");

  Image* img = retrieve(l, 1, AssertOk);

  if(n == 2) {
    QColor col = Wrapper<QColor>::get(l, 2);
    lua_pop(l, n);
    img->clear(col);
  }
  else {
    lua_pop(l, n);
    img->clear();
  }
  return 0;
}

int Wrapper<Image>::setPaintOver(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 2) luaL_error(l, "Wrong argument count for Image::rotate");

  Image* img = retrieve(l, 1, AssertOk);
  bool p = lua_toboolean(l, 2);
  lua_pop(l, n);

  img->setPaintOver(p);
  return 0;
}

int Wrapper<Image>::fillRect(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 3) luaL_error(l, "Wrong argument count for Image::fill_rect");

  Image* img = retrieve(l, 1, AssertOk);
  QRectF* rect = Wrapper<QRectF>::retrieve(l, 2, AssertOk);
  QBrush brush = Wrapper<QBrush>::get(l, 3);
  lua_pop(l, n);

  img->fillRect(*rect, brush);
  return 0;
}

int Wrapper<Image>::drawLine(lua_State* l) {
  const int n = lua_gettop(l);
  if (n > 5 || n < 4) luaL_error(l, "Wrong argument count for Image::draw_line");

  Image* img = retrieve(l, 1, AssertOk);
  QPointF* from = Wrapper<QPointF>::retrieve(l, 2, AssertOk);
  QPointF* to = Wrapper<QPointF>::retrieve(l, 3, AssertOk);
  QColor color = Wrapper<QColor>::get(l, 4);
  double width = (n==5) ? lua_tonumber(l, 5) : 0.0;
  lua_pop(l, n);

  img->drawLine(*from, *to, color, width);
  return 0;
}

int Wrapper<Image>::drawImage(lua_State* l) {
  const int n = lua_gettop(l);
  if (n < 3 || n > 4) luaL_error(l, "Wrong argument count for Image::draw_image");

  Image* img = retrieve(l, 1, AssertOk);
  QRectF* dest = Wrapper<QRectF>::retrieve(l, 2, AssertOk);
  if (lua_isstring(l, 3)) {
    Context* context = retrieve_context(l);
    const char* image_path = lua_tostring(l, 3);
    bool res;
    if (n == 4) {
      QRectF* src = Wrapper<QRectF>::retrieve(l, 4, AssertOk);
      res = img->drawImage(context, *dest, file_path(l, image_path), *src);
    }
    else
      res = img->drawImage(context, *dest, file_path(l, image_path) );

    lua_pop(l, n);
    lua_pushboolean(l, res);
    return 1;
  }
  else {
    Image* image = retrieve(l, 3, AssertOk);
    if (n == 4) {
      QRectF* src = Wrapper<QRectF>::retrieve(l, 4, AssertOk);
      img->drawImage(*dest, *image, *src);
    }
    else
      img->drawImage(*dest, *image);

    lua_pop(l, n);
    return 0;
  }
}

int Wrapper<Image>::drawSVG(lua_State* l) {
  const int n = lua_gettop(l);
  if (n != 3) luaL_error(l, "Wrong argument count for Image::draw_svg");

  Image* img = retrieve(l, 1, AssertOk);
  QRectF* rect = Wrapper<QRectF>::retrieve(l, 2, AssertOk);
  const char* file = lua_tostring(l, 3);
  Context* context = retrieve_context(l);
  lua_pop(l, n);

  bool res = img->drawSVG(context, *rect, file_path(l, file) );
  lua_pushboolean(l, res);
  return 1;
}

int Wrapper<Image>::drawGlyph(lua_State* l) {
#if QT_VERSION >= 0x040200
  int n = lua_gettop(l);
  if (n < 4 || n > 8) luaL_error(l, "Wrong argument count for Image::draw_glyph");

  Image* img = retrieve(l, 1, AssertOk);
  QRectF* rect = Wrapper<QRectF>::retrieve(l, 2, AssertOk);
  const char* font = lua_tostring(l, 3);
  const char* glyph_str = lua_tostring(l, 4);
  if(!font || !glyph_str)
    luaL_error(l, "Null arguments for Image::draw_glyph");

  unsigned int glyph = strtoul(glyph_str, NULL, 0);
  bool draw_inner_bg = true;
  double border = 0.0;
  QBrush fg = Qt::black;
  QBrush bg = Qt::white;

  switch (n) {
  case 8:
    draw_inner_bg = lua_toboolean(l, 8);
  case 7:
    border = lua_tonumber(l, 7);
  case 6:
    bg = Wrapper<QBrush>::get(l, 6);
  case 5:
    fg = Wrapper<QBrush>::get(l, 5);
  default:
    break;
  }
  lua_pop(l, n);

  Context* context = retrieve_context(l);

  bool res = img->drawGlyph(context, *rect, file_path(l, font), glyph,
                                          fg, bg, border, draw_inner_bg);
  lua_pushboolean(l, res);
  return 1;
#else //QT_VERSION >= 0x040200
  lua_pop(l, lua_gettop(l));
  luaL_error(l, "Sorry my dear, you cannot paint font glyphs from images with Qt < 4.2");
  return 0;
#endif
}

int Wrapper<Image>::createShadow(lua_State* l) {
  int n = lua_gettop(l);
  if (n < 3 || n > 5) luaL_error(l, "Wrong argument count for Image::create_shadow");

  Image* img = retrieve(l, 1, AssertOk);
  double radius = lua_tonumber(l, 2);
  QColor color = Wrapper<QColor>::get(l, 3);

  QPoint grow;
  QPointF offset;

  switch (n) {
  case 5:
    offset = *Wrapper<QPointF>::retrieve(l, 5, AssertOk);
  case 4:
    grow = Wrapper<QPointF>::retrieve(l, 4, AssertOk)->toPoint();
  default:
    break;
  }
  lua_pop(l, n);

  allocate(l, new Image(img->createShadow(radius, color, grow, offset)));
  return 1;
}

//END Wrapper<Image> ----------------------------------------------------------

} //end namespace LuaApi

