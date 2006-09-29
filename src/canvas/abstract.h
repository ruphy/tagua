/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_ABSTRACT_H__
#define __CANVAS_ABSTRACT_H__

#include <QList>
#include <QPoint>

class QRect;
class QRegion;

namespace Canvas {

class Item;

/**
    \class Abstract canvas.h <canvas.h>
    \brief Container class.

    A Abstract is a set of canvas items.
*/
class Abstract
{
protected:
    friend class Item;

    QList<Item*> m_items;
    QList<Item*> m_animated_items;

public:
    /** The constructor */
    Abstract();

    virtual ~Abstract();

    /** Returns a const pointer to the list holding all the items in the canvas */
    const QList<Item*>* items() const { return &m_items; };

    /** Helper function to retrieve the topmost item at the given position */
    Item* itemAt(QPoint pos) const;

    /** Overload, same as above */
    Item* itemAt(int x, int y) const { return itemAt(QPoint(x,y)); }

    /** Helper function to retrieve all the items at the given position,
        starting from the topmost one. */
    QList<Item*> itemsAt(QPoint pos) const;

    /** Overload, same as above */
    QList<Item*> itemsAt(int x, int y) const { return itemsAt(QPoint(x,y)); }

    /** Virtual function to let know the canvas that it has animated items in it */
    virtual void ensureAnimating() = 0;

    /** Virtual function to ensure an update is pending, called from children */
    virtual void ensurePendingUpdate() = 0;

    /** Virtual function to update a rect */
    virtual void invalidate(const QRect& r, bool translate = true) = 0;

    /** Virtual function to update a region */
    virtual void invalidate(const QRegion& r, bool translate = true) = 0;

    /** Returns the toplevel non-group Widget object */
    virtual class Widget* topLevelCanvas() = 0;
};

} //end namespace

#endif //__CANVAS_ABSTRACT_H__
