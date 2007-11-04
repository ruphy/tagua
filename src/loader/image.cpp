/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include <QFont>
#include <QFontDatabase>
#include <QPainter>
#include <QPainterPath>
#include <QSvgRenderer>
#include <iostream>
#include "common.h"
#include "imageeffects.h"
#include "loader/context.h"
#include "loader/image.h"

namespace Loader {

//BEGIN Font-------------------------------------------------------------------

Font::Font(int id, int size)
: m_id(id)
, m_families(QFontDatabase::applicationFontFamilies(id))
, m_font(m_families.empty()?QString():m_families[0], size) {
  //for(int i=0;i<m_families.size();i++)
    //std::cout << "Familiy[" << i << "] = " << m_families[i] << std::endl;
}

Font::Font(const QFont& font)
: m_id(-1)
, m_font(font) {

}

Font::~Font() {
  if(m_id != -1)
    QFontDatabase::removeApplicationFont(m_id);
}

FontPtr Font::create(Context* ctx, const QString& file) {
  FontPtr retv;
  if(FontPtr *f = ctx->get<FontPtr>(file))
    retv = *f;
  else {
    int font_id = QFontDatabase::addApplicationFont(file);
    if(font_id != -1) {
      retv = FontPtr(new Font(font_id) );
    }
    ctx->put(file, retv);
  }
  return retv;
}

//END Font---------------------------------------------------------------------


//BEGIN FontGlyph--------------------------------------------------------------

class FontGlyph {
public:
  int          m_height;
  QPainterPath m_path;
  bool         m_inner_background_init;
  QImage       m_inner_background;

  FontGlyph() : m_inner_background_init(false) {}
};

//END FontGlyph----------------------------------------------------------------

typedef boost::shared_ptr<QSvgRenderer> Svg;
typedef boost::shared_ptr<FontGlyph> FontGlyphPtr;


//BEGIN Image------------------------------------------------------------------

Image::Image(int width,
      int height)
: m_image(width, height, QImage::Format_ARGB32_Premultiplied )
, m_draw_opacity(1.0)
, m_draw_over(true) {

  QPainter p(&m_image);
  init_painter(&p);

  p.fillRect(QRect(0,0,width,height), Qt::blue);
}


Image::Image(Context* ctx,
      const QString& file,
      bool use_cache)
: m_draw_opacity(1.0)
, m_draw_over(true) {

  if( QImage *i = ctx->get<QImage>(file) )
    m_image = *i;
  else {
    m_image = QImage(file);
    if(use_cache) ctx->put(file, m_image);
  }
}


void Image::init_painter(QPainter* p) {
  p->setMatrix(m_draw_matrix);
  p->setOpacity(m_draw_opacity);
  p->setRenderHint(QPainter::Antialiasing);
  p->setRenderHint(QPainter::TextAntialiasing);
  p->setRenderHint(QPainter::SmoothPixmapTransform);
  if(m_draw_over)
    p->setCompositionMode(QPainter::CompositionMode_SourceOver);
  else
    p->setCompositionMode(QPainter::CompositionMode_Source);
}


void Image::clear(const QColor& color) {
  QPainter p(&m_image);
  p.setCompositionMode(QPainter::CompositionMode_Source);
  p.fillRect(0,0,width(),height(), color);
}


void Image::fillRect(const QRectF& rect,
              const QBrush& brush) {
  QPainter p(&m_image);
  init_painter(&p);

  p.fillRect(rect, brush);
}


void Image::drawLine(const QPointF& from,
              const QPointF& to,
              const QColor& col,
              double width) {
  QPainter p(&m_image);
  init_painter(&p);

  p.setPen( QPen( col, width ) );
  p.drawLine(from, to);
}


void Image::drawImage(const QRectF& dest,
                const Image& src_img,
                const QRectF& src) {
  QPainter p(&m_image);
  init_painter(&p);

  p.drawImage(dest, src_img.m_image, src.isNull()
                            ? src_img.m_image.rect() : src);
}


bool Image::drawImage(Context* ctx,
                const QRectF& dest,
                const QString& src_img,
                const QRectF& src,
                bool use_cache) {
  Image img(ctx, src_img, use_cache);
  if(img.width()==0 || img.height()==0)
    return false;
  QRectF s = src.isNull() ? QRectF(0,0,img.width(),img.height()) :
      QRectF(src.x()*img.width(),src.y()*img.height(),
             src.width()*img.width(),src.height()*img.height());

  QPainter p(&m_image);
  init_painter(&p);

  p.drawImage(dest, img.m_image, s);
  return true;
}


bool Image::drawSVG(Context* ctx,
              const QRectF& dest,
              const QString& file) {
  Svg svg;

  if( Svg *s = ctx->get<Svg>(file) )
    svg = *s;
  else {
    svg = Svg(new QSvgRenderer(file));
    if(!svg->isValid())
      svg = Svg();
    ctx->put(file, svg);
  }

  if(!svg)
    return false;

  QPainter p(&m_image);
  init_painter(&p);

  p.setViewport(dest.toRect());
  svg->render(&p);
  return true;
}


bool Image::drawGlyph(Context* ctx,
                const QRectF& dest,
                const QString& file,
                unsigned int glyph_num,
                const QBrush& _fg,
                const QBrush& _bg,
                double border,
                bool draw_inner_bg) {
  QChar glyph(glyph_num);
  FontGlyphPtr font_glyph;

  QString k = file + qPrintf(":%04x", glyph_num);
  if(FontGlyphPtr *f = ctx->get<FontGlyphPtr>(k))
    font_glyph = *f;
  else {
    /* get the font (from cache if possible) */
    FontPtr font = Font::create(ctx, file);

    if(!font)
      return false;


    font_glyph = FontGlyphPtr(new FontGlyph);


    /* get a few metrics */
    QFontMetrics fm(font->m_font);

    if(!fm.inFont(glyph))
      return false;

    int h = fm.height();
    int w = fm.width(glyph);
    font_glyph->m_height = h;

    /* create the path from the char */
    font_glyph->m_path.addText((h-w)/2, fm.ascent(), font->m_font, glyph);

    /* save in cache */
    ctx->put(k, font_glyph);
  }

  /* create lazily the inner background region */
  if(!font_glyph->m_inner_background_init && draw_inner_bg
    && (_bg.style() != Qt::NoBrush && !(_bg.style() <= 14 && _bg.color().alpha()==0))) {
    //14 is the last color-based brush

    int h = font_glyph->m_height;
    QImage bg_img(h, h, QImage::Format_ARGB32_Premultiplied );
    bg_img.fill( QColor(Qt::black).rgba() ); //fill all black

    /* render the piece in blue */
    QPainter p;
    p.begin(&bg_img);
    p.setBrush( Qt::blue );
    p.setPen( Qt::NoPen);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawPath(font_glyph->m_path);
    p.end();

    /* flood fill the connected component where blue is <192 with a transparent color */
    ImageEffects::floodFillBlueThreshold(bg_img, QPoint(0,0), Qt::transparent, 192, true);

    /* And we've got our region */
    font_glyph->m_inner_background = bg_img.createAlphaMask();
    font_glyph->m_inner_background.setColor(0, Qt::transparent);
    font_glyph->m_inner_background_init = true;
  }

  /* draw the glyph, at last :) */
  int h = font_glyph->m_height;
  QPainter p(&m_image);
  init_painter(&p);

  p.translate(dest.x(), dest.y());
  p.scale(dest.width()/h, dest.height()/h);

  //14 is the last color-based brush
  if(_bg.style() != Qt::NoBrush && !(_bg.style() <= 14 && _bg.color().alpha()==0)) {
    QBrush bg = _bg;
    QMatrix m = bg.matrix();
    m = m * QMatrix().translate(-dest.x(), -dest.y());
    m = m * QMatrix().scale(h/dest.width(), h/dest.height());
    bg.setMatrix(m);

    if(draw_inner_bg) {
      QMatrix m = p.matrix();
      QRectF r = m.mapRect(QRectF(0,0,h,h));
      QRect ir = r.toRect();
      QImage tmp(ir.size(), QImage::Format_ARGB32_Premultiplied);
      tmp.fill(0);
      {
        QPainter ptmp(&tmp);
        ptmp.setRenderHint(QPainter::Antialiasing);
        ptmp.setRenderHint(QPainter::TextAntialiasing);
        ptmp.setRenderHint(QPainter::SmoothPixmapTransform);
        ptmp.translate(-ir.x(), -ir.y());
        ptmp.setMatrix(m, true);
        ptmp.drawImage(QRect(0,0,h,h), font_glyph->m_inner_background);
        if(border > 0.0 || !_fg.isOpaque() ) {
          ptmp.setBrush( _fg.isOpaque() ? QBrush(Qt::NoBrush) : QBrush(Qt::red) );
          ptmp.setPen( (border > 0.0)
            ? QPen( Qt::green, h*border/100.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
            : Qt::NoPen );
          ptmp.drawPath(font_glyph->m_path);
        }
        ptmp.setCompositionMode(QPainter::CompositionMode_SourceIn);
        ptmp.fillRect(QRect(0,0,h,h), bg);
      }
      p.resetMatrix();
      p.drawImage(ir, tmp);
      p.setMatrix(m);
    }
    else if(border > 0.0 || !_fg.isOpaque() ) {
      p.setBrush( _fg.isOpaque() ? Qt::NoBrush : bg );
      p.setPen( (border > 0.0)
        ? QPen(bg, h*border/100.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin)
        : Qt::NoPen );
      p.drawPath(font_glyph->m_path);
    }
  }
  if(_fg.style() != Qt::NoBrush) {
    QBrush fg = _fg;
    QMatrix m = fg.matrix();
    m = m * QMatrix().translate(-dest.x(), -dest.y());
    m = m * QMatrix().scale(h/dest.width(), h/dest.height());
    fg.setMatrix(m);

    p.setBrush( fg );
    p.setPen( Qt::NoPen );
    p.drawPath(font_glyph->m_path);
  }
  return true;
}


void Image::expBlur(double radius) {
  if(m_image.format() != QImage::Format_RGB32
      && m_image.format() != QImage::Format_ARGB32
      && m_image.format() != QImage::Format_ARGB32_Premultiplied)
    m_image = m_image.convertToFormat(QImage::Format_ARGB32_Premultiplied);
  ImageEffects::expBlur(m_image, int(radius) );
}


Image Image::createShadow(double radius,
                    const QColor& color,
                    const QPoint& grow,
                    const QPointF& offset) {
  Image retv(width()+grow.x(), height()+grow.y());
  int px = int(grow.x()*0.5+offset.x());
  int py = int(grow.y()*0.5+offset.y());
  QPainter p;

  retv.m_image.fill(0);

  p.begin(&retv.m_image);
  p.setCompositionMode(QPainter::CompositionMode_Source);
  p.fillRect( QRect(px, py, width(), height()), color);
  p.setCompositionMode(QPainter::CompositionMode_DestinationAtop );
  p.drawImage( QPoint(px, py), m_image);
  p.end();

  ImageEffects::expBlur(retv.m_image, int(radius) );
  return retv;
}

//END Image--------------------------------------------------------------------

//BEGIN Glyph------------------------------------------------------------------

Glyph::Glyph(const QString& c, int d)
: m_font_valid(false)
, m_char(c)
, m_delta(d) { }

Glyph::Glyph(Context* ctx, const QString& file, const QChar& c, int d)
  : m_font_valid(false)
  , m_char(c)
  , m_delta(d) {
  if(ctx) {
    if(FontPtr ff = Font::create(ctx, file)) {
      m_font_valid = true;
      m_font = ff->m_font;
    }
  }
}

Glyph::Glyph(const QChar& c)
  : m_font_valid(false)
  , m_char(c)
  , m_delta(0) {
}


//END Glyph--------------------------------------------------------------------

} //end namespace Loader
