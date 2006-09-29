/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_WIDGET_H__
#define __CANVAS_WIDGET_H__

#include <QWidget>
//#include <QGLWidget>
#include <boost/shared_ptr.hpp>
#include "canvas/abstract.h"

namespace Canvas {

/**
    \class Widget canvas.h <canvas.h>
    \brief Container widget.

    A Widget is a widget that can contain many Item (images, rectangles, lines, etc).
    Portions of the widget are automatically redrawn to update the changes made to the items.
*/
class Widget : public QWidget, public Abstract
//class Widget : public QGLWidget, public Abstract
{
Q_OBJECT
private:
    friend class Item;
    friend class AnimationNotifier;

    class WidgetPrivate *priv;

    virtual void ensureAnimating();
    virtual void ensurePendingUpdate();
    virtual void invalidate(const QRect& r, bool translate = true);
    virtual void invalidate(const QRegion& r, bool translate = true);

    virtual void paintEvent(QPaintEvent *event);

private slots:
    void processAnimations();
    void updateChanges();

public:
    /** The constructor */
    Widget(QWidget* parent = NULL);

    virtual ~Widget();

    /** Set the delay of the animation, in milliseconds */
    void setAnimationDelay(int d);

    /** Return the number of millisecons from the creation of the canvas
        (see also Item::advance)*/
    int mSecs();

    /** returns 'this' */
    Widget* topLevelCanvas();
};

} //end namespace

#endif //__CANVAS_WIDGET_H__
