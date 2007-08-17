/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "constrainedtext.h"
#include <QApplication>


ConstrainedText::ConstrainedText(const QString& text, const QColor& color,
                        const QFont& font, const QRect& rect,
                        KGameCanvasAbstract* Constrained)
    : KGameCanvasItem(Constrained)
    , m_text(text)
    , m_color(color)
    , m_font(font)
    , m_constr(rect)  {
    calcBoundingRect();
}

ConstrainedText::ConstrainedText(KGameCanvasAbstract* Constrained)
    : KGameCanvasItem(Constrained)
    //, m_text("")
    , m_color(Qt::black)
    , m_font(QApplication::font()) {

}

ConstrainedText::~ConstrainedText() {

}

void ConstrainedText::calcBoundingRect() {
  QString test;
  for(int i=0;i<m_text.length();i++)
    test += 'H';
  m_bounding_rect_max = QFontMetrics(m_font).boundingRect(test);

  m_bounding_rect = QFontMetrics(m_font).boundingRect(m_text);
}

void ConstrainedText::setConstrainRect(const QRect& rect) {
  if(m_constr == rect)
    return;

  m_constr = rect;
  if(visible() && canvas() )
    changed();
}

void ConstrainedText::setText(const QString& text) {
  if(m_text == text)
    return;
  m_text = text;
  calcBoundingRect();

  if(visible() && canvas() )
    changed();
}

void ConstrainedText::setColor(const QColor& color) {
  m_color = color;
}

void ConstrainedText::setFont(const QFont& font) {
  m_font = font;
  calcBoundingRect();

  if(visible() && canvas() )
    changed();
}

void ConstrainedText::paint(QPainter* p) {
  if(m_bounding_rect_max.width() == 0 || m_bounding_rect_max.height() == 0)
    return;

  p->setPen(m_color);
  p->setFont(m_font);

  double fact = qMin(double(m_constr.width())/m_bounding_rect_max.width(),
                      double(m_constr.height())/m_bounding_rect_max.height());
  QMatrix savem = p->matrix();
  //p->fillRect( m_constr, Qt::blue );
  p->translate(QRectF(m_constr).center());
  p->scale(fact, fact);
  p->translate(-QRectF(m_bounding_rect_max).center());
  //p->fillRect( m_bounding_rect_max, Qt::red );
  p->drawText( QPoint((m_bounding_rect_max.width()-m_bounding_rect.width())/2,0), m_text);
  p->setMatrix(savem);
}

QRect ConstrainedText::rect() const {
    return m_constr; //suboptimal. oh, well...
}
