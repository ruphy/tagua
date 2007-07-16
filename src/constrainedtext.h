/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CONSTRAINEDTEXT_H
#define CONSTRAINEDTEXT_H

#include <kgamecanvas.h>

class ConstrainedText : public KGameCanvasItem
{
private:
    QString m_text;
    QColor m_color;
    QFont m_font;
    QRect m_constr;
    QRect m_bounding_rect;
    QRect m_bounding_rect_max;

    void calcBoundingRect();

public:
    ConstrainedText(const QString& text, const QColor& color,
                    const QFont& font, const QRect& rect,
                    KGameCanvasAbstract* canvas = NULL);

    ConstrainedText(KGameCanvasAbstract* canvas = NULL);

    virtual ~ConstrainedText();

    QRect constrainRect() const { return m_constr; }
    void setConstrainRect(const QRect& );
    QString text() const { return m_text; }
    void setText(const QString& text);
    QColor color() const { return m_color; }
    void setColor(const QColor& color);
    QFont font() const { return m_font; }
    void setFont(const QFont& font);

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
    virtual bool layered() const { return false; }
};

#endif //CONSTRAINEDTEXT_H
