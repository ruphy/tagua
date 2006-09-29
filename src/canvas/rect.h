/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_RECT_H__
#define __CANVAS_RECT_H__

#include <QColor>
#include "canvas/item.h"

namespace Canvas {

/**
    \class Rectangle canvas.h <canvas.h>
    \brief A solid rectangle.

    A Pixmap is a pixmap that can be put in the canvas.
*/
class Rectangle : public Item
{
private:
    QColor m_color;
    QSize m_size;

public:
    /** Constructor, specifying the pixmap and the parameters to use */
    Rectangle(const QColor& color, QSize size, Abstract* canvas = NULL);

    /** Constructor, creating with an empty pixmap */
    Rectangle(Abstract* canvas = NULL);

    virtual ~Rectangle();

    /** Returns the color */
    QColor color() const { return m_color; }

    /** Sets the color */
    void setColor(const QColor& color);

    /** Sets the size */
    void setSize(QSize size);

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
    virtual bool layered() const;
};

} //end namespace

#endif //__CANVAS_RECT_H__
