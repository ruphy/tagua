/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_ITEM_H__
#define __CANVAS_ITEM_H__

#include <QRect>
#include <QPoint>

class QPixmap;
class QPainter;
class QRegion;

namespace Canvas {

class Widget;
class Abstract;

/**
    \class Item canvas.h <canvas.h>
    \brief An abstract item.

    A Item is an abstract class to represent a generic item that can be
    put in a canvas.
*/
class Item
{
private:
    friend class Abstract;
    friend class Widget;
    friend class Group;

    bool m_visible;
    bool m_animated;
    int  m_opacity;
    QPoint m_pos;
    Abstract *m_canvas;

    bool m_changed;
    QRect m_last_rect;

    static QPixmap* transparence_pixmap_cache;
    static QPixmap* getTransparenceCache(QSize s);
    virtual void paintInternal(QPainter* p, const QRect& prect, const QRegion& preg,
                                          QPoint delta, double cumulative_opacity);

    void updateAfterRestack(int from, int to);

    /* function to update pending changes, called from parent */
    virtual void updateChanges();

public:
    /** Constructor, it allows you to specify the reference canvas or to create
        an orphan item that will be put into a canvas in a second moment.
        The item is always hidden after beeing created.
        Note that the restacking functions are quite intelligent and will only
        repaint if there is an actual need of doing it. So if you call raise on
        an item that is already (locally) on the top, no redraw will take place */
    Item(Abstract* canvas = NULL);

    virtual ~Item();

    /** schedule an update of the item */
    virtual void changed();

    /** Returns true if the item is visible */
    bool visible() const { return m_visible; }

    /** Set the item as visible or hidden */
    void setVisible(bool v);

    /** Returns true if the item is animated */
    bool animated() const { return m_animated; }

    /** Set the item as animated or not */
    void setAnimated(bool a);

    /** Returns the opacity of the item */
    int opacity() const { return m_opacity; }

    /** Set the item's opacity value (int the 0-255 range) */
    void setOpacity(int o);

    /** Hides the item */
    void hide(){ setVisible(false); }

    /** Shows the item */
    void show(){ setVisible(true); }

    /** Restacks the item on the top of the canvas */
    void raise();

    /** Restacks the item on the bottom of the canvas */
    void lower();

    /** Restacks the item immediately over ref */
    void stackOver(Item* ref);

    /** Restacks the item immediately under ref */
    void stackUnder(Item* ref);

    /** Returns the canvas that is actually "owning" the item */
    Abstract *canvas() const { return m_canvas; }

    /** Returns the toplevel canvas widget, or NULL */
    Widget *topLevelCanvas() const;

    /** Lets you specify the owning canvas. Call this function with canvas
        set to NULL to remove the item from the current canvas. */
    void putInCanvas(Abstract *canvas);

    /** Returns the position of the item */
    QPoint pos() const { return m_pos; }

    /** Sets a new position. Note that an update will be posted to the parent
        canvas, and if you move an item twice in very little time, a region
        bigger than needed will be updated, causing a possible inefficiency */
    void moveTo(QPoint newpos);

    /** Overload, same as above */
    void moveTo(int x, int y) { moveTo( QPoint(x,y)); }

    /** Override this function to draw the item with the painter */
    virtual void paint(QPainter* p) = 0;

    /** Override this function to return the rect the item will be drawn into */
    virtual QRect rect() const = 0;

    /** Override this function to specify if the painting operations will paint over
        each other. If not, the item will be drawn more quickly when opacity is != 255,
        because it does not have to be painted onto a pixmap first. If you don't care
        about the item's opacity, don't care about this function as well. */
    virtual bool layered() const;

    /** Override this function to handle animations, the default function does nothing.
        The argument is the number of milliseconds from the creation of the canvas, so
        that you use it to handle the animation. */
    virtual void advance(int msecs);
};


/**
    \class Dummy canvas.h <canvas.h>
    \brief A dummy (empty) item.

    A Dummy is an empty, invisible item.
    You can use it as reference for stacking items in the canvas using the
    stackOver and stackUnder functions.
*/
class Dummy : public Item
{
public:
    /** Constructor */
    Dummy(Abstract* canvas = NULL);

    virtual ~Dummy();

    /** This function does nothing (of course) */
    virtual void paint(QPainter* p);

    /** This returns an empty rectangle */
    virtual QRect rect() const;
};

} //end namespace

#endif //__CANVAS_ITEM_H__
