/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#ifndef __CANVAS_PIXMAP_H__
#define __CANVAS_PIXMAP_H__

#include <QPixmap>
#include "canvas/item.h"

namespace Canvas {

/**
    \class Pixmap canvas.h <canvas.h>
    \brief A pixmap (sprite).

    A Pixmap is a pixmap that can be put in the canvas.
*/
class Pixmap : public Item
{
private:
    QPixmap m_pixmap;

public:
    /** Constructor, specifying the pixmap to use */
    Pixmap(const QPixmap& pixmap, Abstract* canvas = NULL);

    /** Constructor, creating with an empty pixmap */
    Pixmap(Abstract* canvas = NULL);

    virtual ~Pixmap();

    /** Returns the pixmap */
    QPixmap pixmap() const { return m_pixmap; }

    /** Sets the pixmap of the sprite */
    void setPixmap(const QPixmap& pixmap);

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
    virtual bool layered() const;
};

/**
    \class TiledPixmap canvas.h <canvas.h>
    \brief A tiled pixmap (brush).

    A Pixmap is a pixmap that can be put in the canvas.
*/
class TiledPixmap : public Item
{
private:
    QPixmap m_pixmap;
    QSize m_size;
    QPoint m_origin;
    bool m_move_orig;

public:
    /** Constructor, specifying the pixmap and the parameters to use */
    TiledPixmap(const QPixmap& pixmap, QSize size, QPoint origin,
                            bool move_orig, Abstract* canvas = NULL);

    /** Constructor, creating with an empty pixmap */
    TiledPixmap(Abstract* canvas = NULL);

    virtual ~TiledPixmap();

    /** Returns the pixmap */
    QPixmap pixmap() const { return m_pixmap; }

    /** Sets the pixmap of the tile */
    void setPixmap(const QPixmap& pixmap);

    /** Sets the size */
    void setSize(QSize size);

    /** The origin */
    QPoint origin() const { return m_move_orig ? m_origin + pos() : m_origin; }

    /** Sets the origin of the tiles */
    void setOrigin(QPoint size);

    /** If the origin is moved */
    bool moveOrigin(){ return m_move_orig; }

    /** Sets if the origin of the brush will be moved with the pixmap */
    void setMoveOrigin(bool move_orig);

    virtual void paint(QPainter* p);
    virtual QRect rect() const;
    virtual bool layered() const;
};

} //end namespace

#endif //__CANVAS_PIXMAP_H__
