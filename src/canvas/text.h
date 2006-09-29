/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_TEXT_H__
#define __CANVAS_TEXT_H__

#include <QFont>
#include <QColor>
#include <QString>
#include "canvas/item.h"

namespace Canvas {

/**
    \class Text canvas.h <canvas.h>
    \brief Text.

    A Text is a text that can be put in the canvas.
*/
class Text : public Item
{
public:
    /** Specifies the meaning of the x coordinate of the item. It can
        refer to the start of the text, of the left, center, right of
        the bounding rectangle. */
    enum HPos {
        HStart,
        HLeft,
        HRight,
        HCenter
    };

    /** Specifies the meaning of the y coordinate of the item. It can
        refer to the baseline of the text, of the top, center, bottom of
        the bounding rectangle. */
    enum VPos {
        VBaseline,
        VTop,
        VBottom,
        VCenter
    };

private:
    QString m_text;
    QColor m_color;
    QFont m_font;
    HPos m_hpos;
    VPos m_vpos;
    QRect m_bounding_rect;

    QPoint offsetToDrawPos() const;
    void calcBoundingRect();

public:
    /** Constructor, specifying the text and the parameters to use */
    Text(const QString& text, const QColor& color,
                    const QFont& font, HPos hp, VPos vp,
                    Abstract* canvas = NULL);

    /** Constructor, creating with an empty text */
    Text(Abstract* canvas = NULL);

    virtual ~Text();

    /** Returns the text */
    QString text() const { return m_text; }

    /** Sets the text */
    void setText(const QString& text);

    /** Returns the color */
    QColor color() const { return m_color; }

    /** Sets the color */
    void setColor(const QColor& color);

    /** Returns the font */
    QFont font() const { return m_font; }

    /** Sets the font */
    void setFont(const QFont& font);

    /** Returns the horizontal positioning style */
    HPos hPositioning() const { return m_hpos; }

    /** Returns the vertical positioning style */
    VPos vPositioning() const { return m_vpos; }

    /** Sets the positioning style */
    void setPositioning(HPos hp, VPos vp);

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
    virtual bool layered() const;
};

} //end namespace

#endif //__CANVAS_TEXT_H__
