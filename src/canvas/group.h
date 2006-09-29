/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_GROUP_H__
#define __CANVAS_GROUP_H__

#include "canvas/abstract.h"
#include "canvas/item.h"

namespace Canvas {

/**
    \class Group canvas.h <canvas.h>
    \brief An item containing other items.

    A Group is an Item, but it is also a Abstract,
    so you can add children items to it. Just an inner canvas, if you prefer.
*/
class Group : public Item, public Abstract
{
private:
    mutable bool m_child_rect_changed;
    mutable QRect m_last_child_rect;

    virtual void paintInternal(QPainter* p, const QRect& prect, const QRegion& preg,
                                          QPoint delta, double cumulative_opacity);

    virtual void ensureAnimating();
    virtual void ensurePendingUpdate();
    virtual void invalidate(const QRect& r, bool translate = true);
    virtual void invalidate(const QRegion& r, bool translate = true);
    virtual void updateChanges();
    virtual void changed();

public:
    /** Constructor */
    Group(Abstract* canvas = NULL);

    virtual ~Group();

    /** This paints all the children */
    virtual void paint(QPainter* p);

    /** This returns the bouding rect of all children */
    virtual QRect rect() const;

    /** Animations step, updates the animation for the children */
    virtual void advance(int msecs);

    /** returns the toplevel canvas (or null if it is in an orphan tree) */
    Widget* topLevelCanvas();
};

} //end namespace

#endif //__CANVAS_GROUP_H__
