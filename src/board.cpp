/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <math.h>
#include <iostream>
#include <QPainter>
#include <QApplication>
#include <QMouseEvent>

#include "mastersettings.h"
#include "board.h"
#include "sprite.h"
#include "animation.h"
#include "pointconverter.h"
#include "entities/userentity.h"
#include "mainanimation.h"
#include "premove.h"
#include "constrainedtext.h"

using namespace boost;

/** inherit instead of typedef to ease forward declaration :) */
class BoardTags : public std::map<QString, std::map<Point, boost::shared_ptr<KGameCanvasPixmap> > > {
};

Board::Board(KGameCanvasAbstract* parent)
: ClickableCanvas(parent)
, m_flipped(false)
, m_square_size(0)
, m_border_size(0)
, m_sprites(0,0)
, m_hinting_pos(Point::invalid())
, selection(Point::invalid())
, lastSelection(Point::invalid())
, m_dropped_pool(-1)
, m_dropped_index(-1) {

  m_main_animation = new MainAnimation( 1.0 );

  m_tags = BoardTagsPtr(new BoardTags);

  m_canvas_background = new KGameCanvasGroup(this);
  m_canvas_background->show();

  m_canvas_border = new KGameCanvasGroup(this);
  m_canvas_border->show();

  m_canvas_border_text = new KGameCanvasGroup(this);
  m_canvas_border_text->show();

  m_pieces_group = new KGameCanvasGroup(this);
  m_pieces_group->show();

  settingsChanged();
}

Board::~Board() {
  delete m_pieces_group;

  while(!m_canvas_background->items()->isEmpty())
    delete m_canvas_background->items()->first();
  delete m_canvas_background;

  while(!m_canvas_border->items()->isEmpty())
    delete m_canvas_border->items()->first();
  delete m_canvas_border;

  delete m_main_animation;
}

void Board::settingsChanged() {
  Settings s_anim = settings().group("animations");
  int speed = (s_anim["speed"] | 16);
  int smoothness = (s_anim["smoothness"] | 16);
  m_main_animation->setSpeed( 0.4*pow(10.0, speed/32.0) );
  m_main_animation->setDelay( int(70.0*pow(10.0, -smoothness/32.0)) );

  m_border_text_color = m_controls_loader.getStaticValue<QColor>("border_color");
  m_border_font = m_controls_loader.getStaticValue<QFont>("border_font");

  recreateBorder();
}

void Board::updateBackground() {
  while(!m_canvas_background->items()->isEmpty())
    delete m_canvas_background->items()->first();

  if(!m_square_size)
    return;

  Loader::PixmapOrMap bg = m_tags_loader.getValue<Loader::PixmapOrMap>("background");
  if(const QPixmap* p = boost::get<QPixmap>(&bg)) {
    KGameCanvasTiledPixmap *t = new KGameCanvasTiledPixmap(*p, boardRect().size(), QPoint(),
                                    true, m_canvas_background);
    t->show();
  }
  else if(const Loader::PixmapMap* p = boost::get<Loader::PixmapMap>(&bg)) {
    for(Loader::PixmapMap::const_iterator it = p->begin(); it != p->end(); ++it) {
      KGameCanvasTiledPixmap *t = new KGameCanvasTiledPixmap(it->second, it->first.size(),
                                      QPoint(), true, m_canvas_background);
      t->moveTo(it->first.topLeft());
      t->show();
    }
  }
}

void Board::enqueue(const shared_ptr<Animation>& anim) {
  m_main_animation->addAnimation(anim);
}

void Board::adjustSprite(const Point& p, bool immediate) {
  SpritePtr sprite = m_sprites[p].sprite();

  if(!sprite)
    return;

  enqueue(
    1 /*BROKEN m_anim_movement*/ && !immediate
    ? AnimationPtr(new MovementAnimation(sprite, converter()->toReal(p), 1.0))
    : AnimationPtr(new InstantAnimation(sprite, converter()->toReal(p)))
  );
}

boost::shared_ptr<KGameCanvasPixmap> Board::addTag(const QString& name, Point pt, bool over) {
  if(!m_sprites.valid(pt))
    return boost::shared_ptr<KGameCanvasPixmap>();

  QPixmap p = m_tags_loader(name);
  boost::shared_ptr<KGameCanvasPixmap> item =
      boost::shared_ptr<KGameCanvasPixmap>(new KGameCanvasPixmap(p, this));
  item->moveTo(converter()->toReal(pt));
  if(over)
    item->stackOver(m_pieces_group);
  else
    item->stackUnder(m_pieces_group);
  item->show();

  (*m_tags)[name][pt] = item;
  return item;
}

void Board::clearTags(const QString& name) {
  m_tags->erase(name);
}

void Board::clearTags() {
  m_tags->clear();
}

void Board::setTags(const QString& name, Point p1, Point p2, Point p3,
                                          Point p4, Point p5, Point p6 ) {
  //TODO: maybe this could be optimized a bit
  clearTags(name);
  addTag(name, p1);
  addTag(name, p2);
  addTag(name, p3);
  addTag(name, p4);
  addTag(name, p5);
  addTag(name, p6);
}

void Board::recreateBorder() {
  m_border_text.clear();
  while(!m_canvas_border_text->items()->isEmpty())
    delete m_canvas_border_text->items()->first();

  if(m_border_coords.size() == 0)
    return;

  Point s = m_sprites.getSize();
  for(int w = 0; w<2; w++)
  for(int i = 0;i<s.x;i++) {
    int c = w ? i : i+s.x+s.y;
    QString l = m_border_coords.size()>c ? m_border_coords[c] : QString();
    ConstrainedText *item = new ConstrainedText(m_canvas_border_text);
    item->setColor(m_border_text_color);
    item->setText(l);
    item->setFont(m_border_font);
    item->setColor(m_border_text_color);
    item->show();
    m_border_text.push_back(item);
  }

  for(int w = 0; w<2; w++)
  for(int i = 0;i<s.y;i++) {
    int c = w ? i+s.x : i+2*s.x+s.y;
    QString n = m_border_coords.size()>c ? m_border_coords[c] : QString();
    ConstrainedText *item = new ConstrainedText(m_canvas_border_text);
    item->setColor(m_border_text_color);
    item->setText(n);
    item->setFont(m_border_font);
    item->setColor(m_border_text_color);
    item->show();
    m_border_text.push_back(item);
  }

  m_pieces_group->raise();

  updateBorder();
}

void Board::updateBorder() {
  while(!m_canvas_border->items()->isEmpty())
    delete m_canvas_border->items()->first();

  if(!m_square_size)
    return;

  int at = 0;
  for(int w = 0; w<2; w++)
  for(int i = 0;i<m_sprites.getSize().x;i++) {
    int x = (m_flipped  ?  (m_sprites.getSize().x-1-i)  :  i)*m_square_size;
    int y = w  ?  -m_border_text_far  :  m_square_size*m_sprites.getSize().y+m_border_text_near;

    m_border_text[at]->setVisible(m_border_text_near != m_border_text_far);
    m_border_text[at]->setConstrainRect(QRect(x,y,m_square_size,m_border_text_far-m_border_text_near));
    at++;
  }

  for(int w = 0; w<2; w++)
  for(int i = 0;i<m_sprites.getSize().y;i++) {
    int x = w  ?  (-m_border_text_far-m_border_text_near)/2
               :  m_square_size*m_sprites.getSize().x + (m_border_text_far+m_border_text_near)/2;
    int y = (!m_flipped  ?  (m_sprites.getSize().y-1-i)  :  i)*m_square_size
                + (m_square_size-m_border_text_far-m_border_text_near)/2;

    m_border_text[at]->setVisible(m_border_text_near != m_border_text_far);
    m_border_text[at]->setConstrainRect(QRect(x-m_square_size/2,y,m_square_size,m_border_text_far-m_border_text_near));
    at++;
  }

  ::LuaApi::LuaValueMap params;
  params["width"] = m_square_size*m_sprites.getSize().x;
  params["height"] = m_square_size*m_sprites.getSize().y;
  Loader::PixmapOrMap bord = m_controls_loader.getValue<Loader::PixmapOrMap>("border", &params);
  if(const QPixmap* p = boost::get<QPixmap>(&bord)) {
    KGameCanvasTiledPixmap *t = new KGameCanvasTiledPixmap(*p, boardRect().size(), QPoint(),
                                    true, m_canvas_border);
    t->show();
  }
  else if(const Loader::PixmapMap* p = boost::get<Loader::PixmapMap>(&bord)) {
    for(Loader::PixmapMap::const_iterator it = p->begin(); it != p->end(); ++it) {
      KGameCanvasTiledPixmap *t = new KGameCanvasTiledPixmap(it->second, it->first.size(),
                                      QPoint(), true, m_canvas_border);
      t->moveTo(it->first.topLeft());
      t->show();
    }
  }
}

void Board::createGrid(Point p, const QStringList& border_coords) {
  m_border_coords = border_coords;
  m_sprites = PieceGrid(p.x,p.y);
  recreateBorder();
}

QRect Board::computeRect(Point p) const {
  QPoint realPoint = converter()->toReal(p);
  return squareRect(realPoint.x(), realPoint.y());
}

QRect Board::squareRect(int x, int y) const {
  return QRect(x, y, m_square_size, m_square_size);
}

QRegion Board::computeRegion(Point p) const {
  return QRegion(computeRect(p));
}

// selection
void Board::setSelection(const Point& p) {
  lastSelection = selection;
  selection = p;
  setTags("selection", p);
}

void Board::cancelSelection() {
  lastSelection = selection;
  selection = Point::invalid();
  clearTags("selection");
}

// premove

void Board::setPremove(const NormalUserMove& premove) {
  m_premove_from = premove.from;
  m_premove_to = premove.to;
  setTags("premove", m_premove_from, m_premove_to);
}

void Board::setPremove(const DropUserMove& premove) {
  m_premove_from = Point::invalid();
  m_premove_to = premove.m_to;
  setTags("premove", m_premove_to);
}

void Board::setPremove(const Premove& premove) {
  setPremove(premove.toUserMove());
}

void Board::cancelPremove() {
  m_premove_from = Point::invalid();
  m_premove_to = Point::invalid();
  clearTags("premove");
}

void Board::updateSprites() {
  // adjust piece positions
  for (Point i = m_sprites.first(); i <= m_sprites.last(); i = m_sprites.next(i)) {
    boost::shared_ptr<Sprite> p = m_sprites[i].sprite();

    if (p) {
      // drawing sprite
      p->setPixmap( m_loader( m_sprites[i].name() ) );
      adjustSprite(i, true);
    }
  }
}

void Board::updateTags() {
  if(!m_square_size)
    return;

  for(BoardTags::iterator tit = m_tags->begin(); tit != m_tags->end(); ++tit)
  for(std::map<Point, boost::shared_ptr<KGameCanvasPixmap> >::iterator pt =
                          tit->second.begin(); pt != tit->second.end(); ++pt) {
    pt->second->moveTo(converter()->toReal(pt->first));
    pt->second->setPixmap(m_tags_loader(tit->first));
  }
}


bool Board::doMove(const NormalUserMove& m) {
  if (m_entity.lock()->oneClickMoves() || m_entity.lock()->validTurn(m.from) == Moving) {
    AbstractMove::Ptr mv = m_entity.lock()->testMove(m);
    if (mv) {
      m_entity.lock()->executeMove(mv);
      return true;
    }
  }

  std::cout << "invalid move" << std::endl;
  emit error(InvalidMove);

  return false;
}


void Board::onResize(int new_size, int border_size, int border_text_near,
                                   int border_text_far, bool force_reload) {
  if(m_square_size == new_size && !force_reload)
    return;

  m_square_size = new_size;
  m_border_size = border_size;
  m_border_text_near = border_text_near;
  m_border_text_far = border_text_far;

  // update the size of the piece loader
  m_loader.setSize(m_square_size);

  // update the size of the tag loader
  m_tags_loader.setSize(m_square_size);

  // update the size of the controls loader
  m_controls_loader.setSize(m_border_size);

  // update canvas background
  updateBackground();

  // update the sprites
  updateSprites();

  // update piece tags
  updateTags();

  // update border
  updateBorder();
}

void Board::onMousePress(const QPoint& pos, int button) {
  Point point = converter()->toLogical(pos);
  if (!m_sprites.valid(point))
      return;

  //BEGIN left click

  if (button == Qt::LeftButton) {
    if (m_entity.lock()->oneClickMoves()) {
      NormalUserMove m(Point::invalid(), point);
      m_entity.lock()->setPromotion(m);
      doMove(m);
    }
    else {
      shared_ptr<Sprite> piece = m_sprites[point].sprite();

      if (piece && m_entity.lock()->movable(point)) {
          cancelSelection();
          m_drag_info = DragInfoPtr(new DragInfo(point, pos, piece,
                            m_entity.lock()->validTurn(point)) );
          piece->raise();
      }

      // if selection is valid, (pre)move to point
      else if (selection != Point::invalid()) {
          piece = m_sprites[selection].sprite();
          Q_ASSERT(piece);
          NormalUserMove m(selection, point);
          m_entity.lock()->setPromotion(m);

          switch(m_entity.lock()->validTurn(selection)) {

            case Moving:
              doMove(m);
              cancelSelection();
              break;

            case Premoving:
              if (m_entity.lock()->testPremove(m)) {
                m_entity.lock()->addPremove(m);
                setPremove(m);
                cancelSelection();
              }
              break;

            default:
              break;
          }
      }
    }
  }

  //END left click

  //BEGIN right click

  else if (button == Qt::RightButton) {
    cancelSelection();
    if (point == m_premove_from || point == m_premove_to)
      cancelPremove();
    m_entity.lock()->handleRightClick(point);
  }

  //END right click
}

void Board::onMouseRelease(const QPoint& pos, int button) {
  Point point = converter()->toLogical(pos);

  //BEGIN left click

  if (button == Qt::LeftButton) {

    if (m_drag_info) {
//      Q_ASSERT(m_drag_info->piece);
      Q_ASSERT(m_drag_info->sprite);
      bool moved = false;

      // remove valid move highlighting
      clearTags("validmove");

      // toggle selection if the piece didn't move
      if (m_drag_info->from == point) {
        if (lastSelection == point)
          cancelSelection();
        else
          setSelection(point);
      }

      else  {
        NormalUserMove m(m_drag_info->from, point, true);
        if (!m_sprites.valid(point))
          m.to = Point::invalid();

        m_entity.lock()->setPromotion(m);

        switch(m_entity.lock()->validTurn(m_drag_info->from)) {

          case Moving:
            if (doMove(m))
              moved = true;
            break;

          case Premoving:
            if (m_entity.lock()->testPremove(m)) {
              m_entity.lock()->addPremove(m);
              setPremove(m);
            }
            break;

          default:
            break;
        }
      }

      shared_ptr<Sprite> s = m_sprites[m_drag_info->from].sprite();
      if (!moved && s && s->pos() != converter()->toReal(m_drag_info->from)) {
        Q_ASSERT(s);
        QPoint real = converter()->toReal(m_drag_info->from);
        if( (point == m_drag_info->from) ? 0/* !m_anim_movement*/ : 0 /* !m_anim_fade*/) //BROKEN
          enqueue(shared_ptr<Animation>(new InstantAnimation(s, real)));
        else if (point == m_drag_info->from)
          enqueue(shared_ptr<Animation>(new MovementAnimation(s, real)));
        else
          enqueue(shared_ptr<Animation>(new TeleportAnimation(s, s->pos(), real)));
      }

      m_drag_info = DragInfoPtr();
    }
  }

  //END left button
}

void Board::onMouseMove(const QPoint& pos, int /*button*/) {
  Point point = converter()->toLogical(pos);

  if (m_drag_info) {
    Q_ASSERT(m_drag_info->sprite);
    // check drag threshold
    if (!m_drag_info->dragStarted) {
      QPoint delta = pos - m_drag_info->real;
      if (delta.x() * delta.x() + delta.y() * delta.y() > DragInfo::DRAG_THRESHOLD) {
        m_drag_info->dragStarted = true;
      }
    }
    if (m_drag_info->dragStarted)
      m_drag_info->sprite->moveTo(pos - QPoint(m_square_size / 2, m_square_size / 2) );

    // highlight valid moves
    NormalUserMove move(m_drag_info->from,  point);
    bool valid = m_sprites.valid(point);
    if (valid) {
      InteractionType action = m_entity.lock()->validTurn(m_drag_info->from);
      if (action == Moving)
        valid = m_entity.lock()->testMove(move);
    }

    if (valid)
      setTags("validmove", point);
    else
      clearTags("validmove");
  }
  else if (m_entity.lock()->oneClickMoves()) {
    if(point == m_hinting_pos)
      return;

    AbstractPiece::Ptr hint;

    if (m_sprites.valid(point)) {
      if (AbstractMove::Ptr move = m_entity.lock()->testMove(
                              NormalUserMove(Point::invalid(), point))) {
        // set move hint
        hint = m_entity.lock()->moveHint(move);
      }
    }

    updateHinting(point, hint);
  }
}

void Board::onPositionChanged() {
  if (m_entity.lock() && m_entity.lock()->oneClickMoves() && m_sprites.valid(m_hinting_pos)) {
    AbstractPiece::Ptr hint;

    if (AbstractMove::Ptr move = m_entity.lock()->testMove(
                            NormalUserMove(Point::invalid(), m_hinting_pos)) ) {
      // set move hint
      hint = m_entity.lock()->moveHint(move);
    }

    updateHinting(m_hinting_pos, hint);
  }
}

void Board::onMouseLeave() {
  updateHinting(Point::invalid(), AbstractPiece::Ptr());
}

void Board::updateHinting(Point pt, AbstractPiece::Ptr piece) {
  if(!m_sprites.valid(pt))
    piece = AbstractPiece::Ptr();

  if(!piece || !m_sprites.valid(pt)) {
    if(m_hinting.sprite()) {
      if(1 /*BROKEN m_anim_fade*/)
        enqueue( boost::shared_ptr<Animation>(new FadeAnimation(m_hinting.sprite(),
                                                        m_hinting.sprite()->pos(), 160, 0)) );
      else
        enqueue( boost::shared_ptr<Animation>(new CaptureAnimation(m_hinting.sprite())) );
    }

    m_hinting_pos = Point::invalid();
    m_hinting = NamedSprite();
  }
  else {
    if(pt == m_hinting_pos) {
      if(!(piece->name() == m_hinting.name())) {
        m_hinting = NamedSprite(piece->name(), m_hinting.sprite());
        m_hinting.sprite()->setPixmap(m_loader(piece->name()));
      }
    }
    else {
      if(m_hinting.sprite()) {
        if(1 /*BROKEN m_anim_fade*/)
          enqueue( boost::shared_ptr<Animation>(new FadeAnimation(m_hinting.sprite(),
                                                          m_hinting.sprite()->pos(), 160, 0)) );
        else
          enqueue( boost::shared_ptr<Animation>(new CaptureAnimation(m_hinting.sprite())) );
      }

      QPixmap pix = m_loader(piece->name());
      SpritePtr sprite(new Sprite(pix, piecesGroup(), converter()->toReal(pt)));
      sprite->setOpacity(160);
      sprite->raise();
      sprite->show();

      m_hinting_pos = pt;
      m_hinting = NamedSprite(piece->name(), sprite);

      /*if(m_anim_fade)
        enqueue( boost::shared_ptr<Animation>(new FadeAnimation(m_hinting.sprite(),
                                                        m_hinting.sprite()->pos(), 0, 160)) );
      else {
        m_hinting.sprite()->setOpacity(160);
        enqueue(boost::shared_ptr<Animation>(new DropAnimation(m_hinting.sprite())) );
      }*/
    }
  }
}

void Board::reset() {
  clearTags();
  cancelSelection();
  cancelPremove();
  m_main_animation->stop();
}

void Board::flip(bool flipped)
{
  if (m_flipped != flipped) {
    m_flipped = flipped;

    // update sprite positions
    for (Point i = m_sprites.first(); i <= m_sprites.last(); i = m_sprites.next(i))
    if (m_sprites[i].sprite())
      adjustSprite(i);

    updateTags();
    updateBorder();
  }
}

void Board::draggingOn(int pool, int index, const QPoint& point) {
  Point to = converter()->toLogical(point);

  if (m_sprites.valid(to))
  switch(m_entity.lock()->validTurn(pool)) {
    case Moving: {
      DropUserMove m(pool, index, to);
      AbstractMove::Ptr mv = m_entity.lock()->testMove(m);
      if (mv) {
        setTags("validmove", to);
        return;
      }
      break;
    }

    case Premoving:
      setTags("validmove", to);
      return;

    default:
      break;
  }

  clearTags("validmove");
}

bool Board::dropOn(int pool, int index, const QPoint& point) {

  Point to = converter()->toLogical(point);
  if (!m_sprites.valid(to))
    return false;

  clearTags("validmove");

  switch(m_entity.lock()->validTurn(pool)) {

    case Moving: {
      DropUserMove m(pool, index, to);
      AbstractMove::Ptr mv = m_entity.lock()->testMove(m);
      if (mv)  {
          m_entity.lock()->executeMove(mv);
          return true;
      }
      break;
    }

    case Premoving: {
      DropUserMove m(pool, index, to);
      if (m_entity.lock()->testPremove(m)) {
        m_entity.lock()->addPremove(m);
        setPremove(m);
      }
      break;
    }

    default:
      break;
  }

  std::cout << "invalid move" << std::endl;
  emit error(InvalidMove);
  return false;
}

