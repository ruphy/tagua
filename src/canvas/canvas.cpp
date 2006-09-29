/*
 *  Author: Maurizio Monge <maurizio.monge@gmail.com>
 */

#include <QPaintEvent>
#include <QPainter>
#include <QRegion>
#include <QApplication>
#include <QTimer>
#include <QTime>
#include "canvas/canvas.h"

namespace Canvas {

/*
  TODO:
    - (maybe) allow an item to be destroyed while calling Item::advance.
    - When a group is hidden/destroyed should only update items (optimize for sparse groups)
*/

#define DEBUG_DONT_MERGE_UPDATES 0
#define DEBUG_CANVAS_PAINTS      0


//BEGIN Abstract---------------------------------------------------------------

Abstract::Abstract() {

}

Abstract::~Abstract() {
  for(int i=0;i<m_items.size();i++)
    m_items[i]->m_canvas = NULL;
}

Item* Abstract::itemAt(QPoint pt) const {
  for(int i=m_items.size()-1;i>=0;i--) {
    Item *el = m_items[i];
    if(el->m_visible && el->rect().contains(pt))
      return el;
  }
  return NULL;
}

QList<Item*> Abstract::itemsAt(QPoint pt) const {
  QList<Item*> retv;

  for(int i=m_items.size()-1;i>=0;i--) {
    Item *el = m_items[i];
    if(el->m_visible && el->rect().contains(pt))
      retv.append(el);
  }

  return retv;
}

//END Abstract-----------------------------------------------------------------


//BEGIN Widget-----------------------------------------------------------------

class WidgetPrivate {
public:
  QTimer m_anim_timer;
  QTime m_anim_time;
  bool m_pending_update;
  QRegion m_pending_update_reg;

#if DEBUG_CANVAS_PAINTS
  bool debug_paints;
#endif //DEBUG_CANVAS_PAINTS

  WidgetPrivate()
  : m_pending_update(false)
#if DEBUG_CANVAS_PAINTS
  , debug_paints(false)
#endif //DEBUG_CANVAS_PAINTS
  {}
};

Widget::Widget(QWidget* parent)
: QWidget(parent)
//: QGLWidget(parent)
, priv(new WidgetPrivate()) {
  priv->m_anim_time.start();
  connect(&priv->m_anim_timer, SIGNAL(timeout()), this, SLOT(processAnimations()));
}

Widget::~Widget() {
  delete priv;
}

void Widget::ensureAnimating() {
  if(!priv->m_anim_timer.isActive() )
      priv->m_anim_timer.start();
}

void Widget::ensurePendingUpdate() {
  if(priv->m_pending_update)
    return;
  priv->m_pending_update = true;

#if DEBUG_DONT_MERGE_UPDATES
  updateChanges();
#else //DEBUG_DONT_MERGE_UPDATES
  QTimer::singleShot( 0, this, SLOT(updateChanges()) );
#endif //DEBUG_DONT_MERGE_UPDATES
}

void Widget::updateChanges() {
  for(int i=0;i<m_items.size();i++) {
    Item *el = m_items[i];

    if(el->m_changed)
      el->updateChanges();
  }
  priv->m_pending_update = false;

#if DEBUG_CANVAS_PAINTS
  repaint();
  priv->debug_paints = true;
  repaint( priv->m_pending_update_reg );
  QApplication::syncX();
  priv->debug_paints = false;
  usleep(100000);
  repaint( priv->m_pending_update_reg );
  QApplication::syncX();
  usleep(100000);
#else //DEBUG_CANVAS_PAINTS
  repaint( priv->m_pending_update_reg );
#endif //DEBUG_CANVAS_PAINTS

  priv->m_pending_update_reg = QRegion();
}

void Widget::invalidate(const QRect& r, bool /*translate*/) {
  priv->m_pending_update_reg |= r;
  ensurePendingUpdate();
}

void Widget::invalidate(const QRegion& r, bool /*translate*/) {
  priv->m_pending_update_reg |= r;
  ensurePendingUpdate();
}

void Widget::paintEvent(QPaintEvent *event) {
#if DEBUG_CANVAS_PAINTS
  if(priv->debug_paints)
  {
    QPainter p(this);
    p.fillRect(event->rect(), Qt::magenta);
    return;
  }
#endif //DEBUG_CANVAS_PAINTS

  {QPainter p(this);
  QRect evr = event->rect();
  QRegion evreg = event->region();

  for(int i=0;i<m_items.size();i++) {
    Item *el = m_items[i];
    if( el->m_visible && evr.intersects( el->rect() )
        && evreg.contains( el->rect() ) ) {
      el->m_last_rect = el->rect();
      el->paintInternal(&p, evr, evreg, QPoint(), 1.0 );
    }
  }}

  QApplication::syncX();
}

void Widget::processAnimations() {
  if(m_animated_items.empty() ) {
    priv->m_anim_timer.stop();
    return;
  }

  int tm = priv->m_anim_time.elapsed();
  QList<Item*> ait = m_animated_items;
  for(int i=0;i<ait.size();i++) {
    Item *el = ait[i];
    el->advance(tm);
  }

  if(m_animated_items.empty() )
    priv->m_anim_timer.stop();
}

void Widget::setAnimationDelay(int d) {
  priv->m_anim_timer.setInterval(d);
}

int Widget::mSecs() {
  return priv->m_anim_time.elapsed();
}

Widget* Widget::topLevelCanvas() {
  return this;
}

//END Widget-------------------------------------------------------------------


//BEGIN Item-------------------------------------------------------------------

Item::Item(Abstract* canvas)
: m_visible(false)
, m_animated(false)
, m_opacity(255)
, m_pos(0,0)
, m_canvas(canvas)
, m_changed(false) {
  if(m_canvas) m_canvas->m_items.append(this);
}

Item::~Item() {
  if(m_canvas) {
    m_canvas->m_items.removeAll(this);
    if(m_animated)
      m_canvas->m_animated_items.removeAll(this);
    if(m_visible)
      m_canvas->invalidate(m_last_rect, false);
  }
}

Widget* Item::topLevelCanvas() const {
  return m_canvas ? m_canvas->topLevelCanvas() : NULL;
}

void Item::changed() {
  m_changed = true;
  if(m_canvas)
    m_canvas->ensurePendingUpdate();
}

void Item::updateChanges() {
  if(!m_changed)
    return;
  if(m_canvas) {
    m_canvas->invalidate(m_last_rect, false);
    if(m_visible)
      m_canvas->invalidate(rect());
  }
  m_changed = false;
}

QPixmap *Item::transparence_pixmap_cache = NULL;

QPixmap* Item::getTransparenceCache(QSize s) {
  if(!transparence_pixmap_cache)
    transparence_pixmap_cache = new QPixmap();
  if(s.width()>transparence_pixmap_cache->width() ||
    s.height()>transparence_pixmap_cache->height()) {

    /* Strange that a pixmap with alpha should be created this way, i think a qt bug */
    *transparence_pixmap_cache = QPixmap::fromImage(
              QImage( s.expandedTo(transparence_pixmap_cache->size()),
                              QImage::Format_ARGB32_Premultiplied ) );
  }

  return transparence_pixmap_cache;
}

void Item::paintInternal(QPainter* pp, const QRect& /*prect*/,
                    const QRegion& /*preg*/, QPoint /*delta*/, double cumulative_opacity) {
  int opacity = int(cumulative_opacity*m_opacity + 0.5);

  if(opacity <= 0)
    return;

  if(opacity >= 255) {
    paint(pp);
    return;
  }

#if QT_VERSION >= 0x040200
  if(!layered()) {
    pp->setOpacity(opacity/255.0);
    paint(pp);
    pp->setOpacity(1.0);
    return;
  }
#endif

  QRect mr = rect();
  QPixmap* cache = getTransparenceCache(mr.size());

  {
    QPainter p(cache);

    /* clear */
    p.setBrush(QColor(255,255,255,0));
    p.setPen(Qt::NoPen);
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.drawRect(QRect(QPoint(),mr.size()));

    /* paint on the item */
    p.translate(-mr.topLeft());
    paint(&p);
    p.translate(mr.topLeft());

    /* make the opacity */
    p.setBrush( QColor(255,255,255,255-opacity) );
    p.setPen( Qt::NoPen );
    p.setCompositionMode( QPainter::CompositionMode_DestinationOut );
    p.drawRect( QRect(QPoint(),mr.size()) );
  }

  pp->drawPixmap(mr.topLeft(), *cache, QRect(QPoint(),mr.size()) );
}

void Item::putInCanvas(Abstract *c) {
  if(m_canvas == c)
      return;

  if(m_canvas) {
    if(m_visible)
      m_canvas->invalidate(m_last_rect, false);
    m_canvas->m_items.removeAll(this);
    if(m_animated)
      m_canvas->m_animated_items.removeAll(this);
  }

  m_canvas = c;

  if(m_canvas) {
    m_canvas->m_items.append(this);
    if(m_animated) {
      m_canvas->m_animated_items.append(this);
      m_canvas->ensureAnimating();
    }
    if(m_visible)
      m_canvas->invalidate(rect());
  }
}

void Item::setVisible(bool v) {
  if(m_visible == v)
      return;

  m_visible = v;
  if(m_canvas) {
    if(!v)
      m_canvas->invalidate(m_last_rect, false);
    else
      changed();
  }
  if(!v)
    m_last_rect = QRect();
}

void Item::setAnimated(bool a) {
  if(m_animated == a)
    return;

  m_animated = a;
  if(m_canvas) {
    if(a) {
      m_canvas->m_animated_items.append(this);
      m_canvas->ensureAnimating();
    }
    else
      m_canvas->m_animated_items.removeAll(this);
  }
}

void Item::setOpacity(int o) {
  m_opacity = o;

  if(m_canvas && m_visible)
    changed();
}

bool Item::layered() const { return true; }

void Item::advance(int /*msecs*/) { }

void Item::updateAfterRestack(int from, int to)
{
    int inc = from>to ? -1 : 1;

    QRegion upd;
    for(int i=from; i!=to;i+=inc)
    {
        Item *el = m_canvas->m_items[i];
        if(!el->m_visible)
            continue;

        QRect r = el->rect() & rect();
        if(!r.isEmpty())
            upd |= r;
    }

    if(!upd.isEmpty())
        m_canvas->invalidate(upd);
}

void Item::raise()
{
    if(!m_canvas || m_canvas->m_items.last() == this)
        return;

    int old_pos = m_canvas->m_items.indexOf(this);
    m_canvas->m_items.removeAt(old_pos);
    m_canvas->m_items.append(this);
    if(m_visible)
        updateAfterRestack(old_pos, m_canvas->m_items.size()-1);
}

void Item::lower()
{
    if(!m_canvas || m_canvas->m_items.first() == this)
        return;

    int old_pos = m_canvas->m_items.indexOf(this);
    m_canvas->m_items.removeAt(old_pos);
    m_canvas->m_items.prepend(this);

    if(m_visible)
        updateAfterRestack(old_pos, 0);
}

void Item::stackOver(Item* ref)
{
    if(!m_canvas)
        return;

    if(ref->m_canvas != m_canvas)
    {
        qCritical("Item::stackOver: Argument must be a sibling item!\n");
        return;
    }

    int i = m_canvas->m_items.indexOf( ref );
    if(i < m_canvas->m_items.size()-2  &&  m_canvas->m_items[i+1] == this)
        return;

    int old_pos = m_canvas->m_items.indexOf(this);
    m_canvas->m_items.removeAt(old_pos);
    i = m_canvas->m_items.indexOf( ref, i-1 );
    m_canvas->m_items.insert(i+1,this);

    if(m_visible)
        updateAfterRestack(old_pos, i+1);
}

void Item::stackUnder(Item* ref)
{
    if(!m_canvas)
        return;


    if(ref->m_canvas != m_canvas)
    {
        qCritical("Item::stackUnder: Argument must be a sibling item!\n");
        return;
    }

    int i = m_canvas->m_items.indexOf( ref );
    if(i >= 1  &&  m_canvas->m_items[i-1] == this)
        return;

    int old_pos = m_canvas->m_items.indexOf(this);
    m_canvas->m_items.removeAt(old_pos);
    i = m_canvas->m_items.indexOf( ref, i-1 );
    m_canvas->m_items.insert(i,this);

    if(m_visible)
        updateAfterRestack(old_pos, i);
}

void Item::moveTo(QPoint newpos)
{
  if(m_pos == newpos)
    return;
  m_pos = newpos;
  if(m_visible && m_canvas)
    changed();
}

//END Item---------------------------------------------------------------------


//BEGIN Group------------------------------------------------------------------

Group::Group(Abstract* canvas)
: Item(canvas)
, Abstract()
, m_child_rect_changed(true) {

}

Group::~Group() {

}

void Group::ensureAnimating() {
  setAnimated(true);
}

void Group::ensurePendingUpdate() {
  if(!m_changed || !m_child_rect_changed) {
    m_child_rect_changed = true;
    Item::changed();
  }
}

void Group::updateChanges() {
  if(!m_changed)
    return;
  for(int i=0;i<m_items.size();i++) {
    Item *el = m_items[i];

    if(el->m_changed)
      el->updateChanges();
  }
  m_changed = false;
}

void Group::changed() {
  m_child_rect_changed = true;

  Item::changed();

  for(int i=0;i<m_items.size();i++)
    m_items[i]->changed();
}

void Group::invalidate(const QRect& r, bool translate) {
  if(m_canvas)
    m_canvas->invalidate(translate ? r.translated(m_pos) : r, translate);
  if(!m_changed)
    ensurePendingUpdate();
}

void Group::invalidate(const QRegion& r, bool translate) {
  if(m_canvas)
    m_canvas->invalidate(translate ? r.translated(m_pos) : r, translate);
  if(!m_changed)
    ensurePendingUpdate();
}

void Group::advance(int msecs) {
  QList<Item*> ait = m_animated_items;
  for(int i=0;i<ait.size();i++)
  {
      Item *el = ait[i];
      el->advance(msecs);
  }

  if(m_animated_items.empty())
      setAnimated(false);
}

void Group::paintInternal(QPainter* p, const QRect& prect,
          const QRegion& preg, QPoint delta, double cumulative_opacity) {
  cumulative_opacity *= (m_opacity/255.0);

  delta += m_pos;
  p->translate(m_pos);

  for(int i=0;i<m_items.size();i++) {
    Item *el = m_items[i];
    QRect r = el->rect().translated(delta);

    if( el->m_visible && prect.intersects( r ) && preg.contains( r ) ) {
      el->m_last_rect = r;
      el->paintInternal(p,prect,preg,delta,cumulative_opacity);
    }
  }

  p->translate(-m_pos);
}

void Group::paint(QPainter* /*p*/) {
  Q_ASSERT(!"This function should never be called");
}

QRect Group::rect() const
{
  if(!m_child_rect_changed)
    return m_last_child_rect.translated(m_pos);

  m_last_child_rect = QRect();
  for(int i=0;i<m_items.size();i++)
  {
    Item *el = m_items[i];
    if(el->m_visible)
      m_last_child_rect |= el->rect();
  }

  m_child_rect_changed = false;
  return m_last_child_rect.translated(m_pos);
}

Widget* Group::topLevelCanvas()
{
    return m_canvas ? m_canvas->topLevelCanvas() : NULL;
}

//END Group--------------------------------------------------------------------


//BEGIN Dummy------------------------------------------------------------------

Dummy::Dummy(Abstract* canvas)
    : Item(canvas)
{

}

Dummy::~Dummy()
{

}

void Dummy::paint(QPainter* /*p*/) {
}

QRect Dummy::rect() const
{
    return QRect();
}

//END Dummy--------------------------------------------------------------------


//BEGIN Pixmap-----------------------------------------------------------------

Pixmap::Pixmap(const QPixmap& p, Abstract* canvas)
    : Item(canvas), m_pixmap(p) {

}

Pixmap::Pixmap(Abstract* canvas)
    : Item(canvas) {

}

Pixmap::~Pixmap() {

}

void Pixmap::setPixmap(const QPixmap& p) {
  m_pixmap = p;
  if(visible() && canvas() )
    changed();
}

void Pixmap::paint(QPainter* p) {
  p->drawPixmap(pos(), m_pixmap);
}

QRect Pixmap::rect() const {
    return QRect(pos(), m_pixmap.size());
}

bool Pixmap::layered() const {
  return false;
}

//END Pixmap-------------------------------------------------------------------


//BEGIN TiledPixmap------------------------------------------------------------

TiledPixmap::TiledPixmap(const QPixmap& pixmap, QSize size, QPoint origin,
                        bool move_orig, Abstract* canvas)
    : Item(canvas)
    , m_pixmap(pixmap)
    , m_size(size)
    , m_origin(origin)
    , m_move_orig(move_orig) {

}

TiledPixmap::TiledPixmap(Abstract* canvas)
    : Item(canvas)
    , m_size(0,0)
    , m_origin(0,0)
    , m_move_orig(false) {

}

TiledPixmap::~TiledPixmap() {

}

void TiledPixmap::setPixmap(const QPixmap& pixmap) {
  m_pixmap = pixmap;
  if(visible() && canvas() )
    changed();
}

void TiledPixmap::setSize(QSize size) {
  m_size = size;
  if(visible() && canvas() )
    changed();
}

void TiledPixmap::setOrigin(QPoint origin)
{
  m_origin = m_move_orig ? origin - pos() : origin;

  if(visible() && canvas() )
    changed();
}


void TiledPixmap::setMoveOrigin(bool move_orig)
{
  if(move_orig && !m_move_orig)
      m_origin -= pos();
  if(move_orig && !m_move_orig)
      m_origin += pos();
  m_move_orig = move_orig;
}

void TiledPixmap::paint(QPainter* p)
{
    if(m_move_orig)
        p->drawTiledPixmap( rect(), m_pixmap, m_origin);
    else
        p->drawTiledPixmap( rect(), m_pixmap, m_origin+pos() );
}

QRect TiledPixmap::rect() const
{
    return QRect(pos(), m_size);
}

bool TiledPixmap::layered() const {
  return false;
}

//END TiledPixmap--------------------------------------------------------------


//BEGIN Rectangle--------------------------------------------------------------

Rectangle::Rectangle(const QColor& color, QSize size, Abstract* canvas)
    : Item(canvas)
    , m_color(color)
    , m_size(size)
{

}

Rectangle::Rectangle(Abstract* canvas)
    : Item(canvas)
    , m_size(0,0)
{

}

Rectangle::~Rectangle()
{

}

void Rectangle::setColor(const QColor& color)
{
  m_color = color;
  if(visible() && canvas() )
    changed();
}

void Rectangle::setSize(QSize size)
{
  m_size = size;
  if(visible() && canvas() )
    changed();
}

void Rectangle::paint(QPainter* p) {
  p->fillRect( rect(), m_color );
}

QRect Rectangle::rect() const {
    return QRect(pos(), m_size);
}

bool Rectangle::layered() const {
  return false;
}

//END Rectangle----------------------------------------------------------------


//BEGIN Text-------------------------------------------------------------------

Text::Text(const QString& text, const QColor& color,
                        const QFont& font, HPos hp, VPos vp,
                        Abstract* canvas)
    : Item(canvas)
    , m_text(text)
    , m_color(color)
    , m_font(font)
    , m_hpos(hp)
    , m_vpos(vp) {
    calcBoundingRect();
}

Text::Text(Abstract* canvas)
    : Item(canvas)
    , m_text("")
    , m_color(Qt::black)
    , m_font(QApplication::font())
    , m_hpos(HStart)
    , m_vpos(VBaseline) {

}

Text::~Text() {

}

void Text::calcBoundingRect() {
    m_bounding_rect = QFontMetrics(m_font).boundingRect(m_text);
    /*printf("b rect is %d %d %d %d\n",
        m_bounding_rect.x(),
        m_bounding_rect.y(),
        m_bounding_rect.width(),
        m_bounding_rect.height() );*/
}

void Text::setText(const QString& text) {
  if(m_text == text)
    return;
  m_text = text;
  calcBoundingRect();

  if(visible() && canvas() )
    changed();
}

void Text::setColor(const QColor& color) {
  m_color = color;

  if(visible() && canvas() )
    changed();
}

void Text::setFont(const QFont& font) {
  m_font = font;
  calcBoundingRect();

  if(visible() && canvas() )
    changed();
}

void Text::setPositioning(HPos hp, VPos vp) {
  pos() += offsetToDrawPos();
  m_hpos = hp;
  m_vpos = vp;
  pos() -= offsetToDrawPos();
}

QPoint Text::offsetToDrawPos() const {
    QPoint retv;

    switch(m_hpos) {
        case HStart:
            retv.setX(0);
            break;
        case HLeft:
            retv.setX(-m_bounding_rect.left());
            break;
        case HRight:
            retv.setX(-m_bounding_rect.right());
            break;
        case HCenter:
            retv.setX(-(m_bounding_rect.left()+m_bounding_rect.right())/2);
            break;
    }

    switch(m_vpos) {
        case VBaseline:
            retv.setY(0);
            break;
        case VTop:
            retv.setY(-m_bounding_rect.top());
            break;
        case VBottom:
            retv.setY(-m_bounding_rect.bottom());
            break;
        case VCenter:
            retv.setY(-(m_bounding_rect.top()+m_bounding_rect.bottom())/2);
            break;
    }

    return retv;
}

void Text::paint(QPainter* p) {
  p->setPen(m_color);
  p->setFont(m_font);
  p->drawText( pos() + offsetToDrawPos(), m_text);
}

QRect Text::rect() const {
  return m_bounding_rect.translated( pos() + offsetToDrawPos() );
}

bool Text::layered() const {
  return false;
}

//END Text---------------------------------------------------------------------


//BEGIN Picture----------------------------------------------------------------

Picture::Picture(const QPicture& p, Abstract* canvas)
    : Item(canvas), m_picture(p)
{

}

Picture::Picture(Abstract* canvas)
    : Item(canvas)
{

}

Picture::~Picture()
{

}

void Picture::setPicture(const QPicture& p)
{
  m_picture = p;

  if(visible() && canvas() )
    changed();
}

void Picture::paint(QPainter* p)
{
    p->drawPicture(pos(), m_picture);
}

QRect Picture::rect() const
{
    return m_picture.boundingRect().translated( pos());
}

//END Picture------------------------------------------------------------------

} //end namespace Canvas
