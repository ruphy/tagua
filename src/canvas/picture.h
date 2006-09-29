/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_PICTURE_H__
#define __CANVAS_PICTURE_H__

#include <QPicture>
#include "canvas/item.h"

namespace Canvas {

/**
    \class Picture canvas.h <canvas.h>
    \brief A picture, ie a collection of paint operations.

    A Picture is a picture that can be replayed on the canvas.
*/
class Picture : public Item
{
private:
    QPicture m_picture;

public:
    /** Constructor, specifying the picture to use */
    Picture(const QPicture& picture, Abstract* canvas = NULL);

    /** Constructor, creating with an empty picture */
    Picture(Abstract* canvas = NULL);

    virtual ~Picture();

    /** Returns the picture */
    QPicture picture() const { return m_picture; }

    /** Sets the picture of the sprite */
    void setPicture(const QPicture& picture);

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
};

} //end namespace

#endif //__CANVAS_PICTURE_H__
