/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LOADER__IMAGE_H
#define LOADER__IMAGE_H

#include <boost/shared_ptr.hpp>
#include <QColor>
#include <QMatrix>
#include <QBrush>
#include <QFont>
#include <QImage>

class QPainter;

/**
  * @namespace Loader
  * @brief Namespace holding image (and more generally resource) loading functions
  */
namespace Loader {


class Context;


/**
  * @class Image <loader/image.h>
  * @brief The image class to be used with lua
  */
class Image {
private:
  QImage  m_image;
  QMatrix m_draw_matrix;
  double  m_draw_opacity;
  bool    m_draw_over;

  void init_painter(QPainter*);

public:

  /**
   * Creates an image. Image data will be undefined.
   * @param width The width of the image.
   * @param height The height of the image.
   */
  Image(int width,
        int height);


  /**
   * Creates an image from a file. If the files is not found or could not be
   * loaded, the resulting image will have width and height equal to 0
   * @param ctx The current context (theme) reference (should be implicit in lua)
   * @param file The file name.
   */
  Image(Context* ctx,
        const QString& file,
        bool use_cache = true);


  /** \return the QImage */
  QImage image() { return m_image; }

  /** Returns the width of the image */
  int width() {
    return m_image.width();
  }


  /** Returns the height of the image */
  int height() {
    return m_image.height();
  }


  /**
   * Sets the transformation matrix
   */
  void setMatrix(const QMatrix& m) {
    m_draw_matrix = m;
  }


  /**
   * Resets the transformation matrix
   */
  void resetMatrix() {
    m_draw_matrix.reset();
  }


  /**
   * Scales the transformation matrix
   */
  void scale(double x, double y) {
    m_draw_matrix = m_draw_matrix * QMatrix().scale(x,y);
  }


  /**
   * Rotates the transformation matrix
   */
  void rotate(double angle) {
    m_draw_matrix = m_draw_matrix * QMatrix().rotate(angle);
  }


  /**
   * Translates the transformation matrix
   */
  void translate(double x, double y) {
    m_draw_matrix = m_draw_matrix * QMatrix().translate(x,y);
  }


  /**
   * Returns the transformation matrix
   */
  QMatrix matrix() {
    return m_draw_matrix;
  }


  /**
   * Sets the opacity of the drawing operations
   */
  void setOpacity(double o) {
    m_draw_opacity = o;
  }


  /**
   * Returns the opacity of the drawing operations
   */
  double opacity() {
    return m_draw_opacity;
  }


  /**
   * Sets the composition mode
   * @param over If true the painting operations will paint over,
   *               if false will replace the destination color.
   */
  void setPaintOver(bool over) {
    m_draw_over = over;
  }


  /**
   * Clears the image.
   * @param color The color to fill the image with (default transparent)
   */
  void clear(const QColor& color = Qt::transparent);


  /**
   * Fills a rectangle.
   * @param rect The rectangle
   * @param brush The fill brush
   */
  void fillRect(const QRectF& rect,
                const QBrush& brush);


  /**
   * Draws a line.
   * @param from The starting point
   * @param to The end point
   * @param col The pen color
   * @param width The pen width
   */
  void drawLine(const QPointF& from,
                const QPointF& to,
                const QColor& col,
                double width);


  /**
   * Draws an image on the over the current one.
   * @param dest The destination rectangle.
   * @param src_img The source image.
   * @param src The source rectangle.
   */
  void drawImage(const QRectF& dest,
                 const Image& src_img,
                 const QRectF& src = QRectF(0.0,0.0,0.0,0.0));


  /**
   * Draws an image on the over the current one (overload).
   * @param ctx The current context (theme) reference (should be implicit in lua)
   * @param dest The destination rectangle.
   * @param src_img The source image file.
   * @param src The source rectangle, WITH RELATIVE COORDINATES 0.0 - 1.0 (of course).
   * @return True if it was possible to load the image file.
   */
  bool drawImage(Context* ctx,
                 const QRectF& dest,
                 const QString& src_img,
                 const QRectF& src = QRectF(0.0,0.0,0.0,0.0),
                 bool use_cache = true);


  /**
   * Draws an SVG file over the image.
   * @param ctx The current context (theme) reference (should be implicit in lua)
   * @param dest The destination rectangle.
   * @param file The file to load.
   * @return True if it was possible to load the SVG file.
   * TODO: add css support (When Qt will support it)
   */
  bool drawSVG(Context* ctx,
               const QRectF& dest,
               const QString& file);


  /**
   * Draws a font glyph over the image.
   * @param ctx The current context (theme) reference (should be implicit in lua)
   * @param dest The destination rectangle.
   * @param font The font file to load.
   * @param glyph The unicode glyph code.
   * @param fg The foreground color.
   * @param bg The background color.
   * @param border The background expansion.
   * @param draw_inner_bg If true the 'inner part' (detected with a flood fill
                          algorithm) will be filled with the background brush.
   * @return True if it was possible to load the font file and find the glyph.
   */
  bool drawGlyph(Context* ctx,
                 const QRectF& dest,
                 const QString& font,
                 unsigned int glyph,
                 const QBrush& fg = Qt::black,
                 const QBrush& bg = Qt::white,
                 double border = 0.0,
                 bool draw_inner_bg = true);


  /**
   * Blurs the image itself.
   */
  void expBlur(double radius);

  /**
   * Returns a shadow image for the current image.
   * @param radius The shadow radius.
   * @param color  The shadow color.
   * @param grow   How bigger the output image will be.
   * @param offset Position of the shadow (relatively from being centered in the output).
   */
  Image createShadow(double radius,
                     const QColor& color,
                     const QPoint& grow = QPoint(),
                     const QPointF& offset = QPointF() );
};

/**
  * @class Glyph <loader/image.h>
  * @brief A simple class that represents a glyph in a font to be used with lua
  */
class Glyph {
private:
  bool m_font_valid;
  QFont m_font;
  QChar m_char;
  int m_delta;

public:
  Glyph(Context* ctx, const QString&, QChar, int = 0);
  Glyph(QChar = QChar());

  QFont font() { return m_font; }
  bool fontValid() { return m_font_valid; }
  QChar ch() { return m_char; }
  int delta() { return m_delta; }
};


typedef boost::shared_ptr<class Font> FontPtr;

class Font {
private:
  friend class Image;
  friend class Glyph;

  QString     m_file;
  int         m_id;
  QStringList m_families;
  QFont       m_font;

public:
  Font(int id, int size = 128);
  Font(const QFont& font);
  ~Font();

  QFont font() { return m_font; }
  QStringList families() { return m_families; }

  static FontPtr create(Context* ctx, const QString& file);
};


} //end namespace Loader

#endif //LOADER__IMAGE_H
