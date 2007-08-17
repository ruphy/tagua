/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QImage>
#include <QPainter>
#include <iostream>
#include <cmath>
#include <boost/random/uniform_smallint.hpp>
#include <vector>
#include "common.h"
#include "point.h"
#include "sprite.h"


using namespace boost;

struct ExplosionFragment {
  QPoint m_polygon[6];
  int    m_pcount;
  QPoint m_speed;
  ExplosionFragment() : m_pcount(0) {}
};

/* inherit instead of typedef to ease forward declaration :) */
class SpriteExplosion : public std::vector<ExplosionFragment> {

};

Sprite::Sprite(const QPixmap& pix, KGameCanvasAbstract* canvas,
                                              const QPoint& location)
: KGameCanvasPixmap(pix, canvas)
, m_pixmap(pix)
, m_explode_step(0.0)
, m_explosion(NULL)
, m_rotation(0.0)
, m_scale(1.0)
#ifdef DEBUG_PIECE
, m_dummy_opacity(255)
, m_dummy_visible(false)
#endif
{
  moveTo(location);
  lower();

#ifdef DEBUG_PIECE
  update_from_dummy();
#endif
}

Sprite::~Sprite() {
  if(m_explosion)
    delete m_explosion;
}

Sprite* Sprite::duplicate() const {
  return new Sprite(pixmap(), canvas(), pos() );
}

void Sprite::setThumb(const QImage& thumb) {
  std::cout << "setting thumb" << std::endl;
  QPixmap pix = m_pixmap;
  int width = pix.width() / 2;
  int height = pix.height() / 2;
  QPixmap thumb_pix = QPixmap::fromImage(
    thumb.scaled(width, height,
              Qt::IgnoreAspectRatio, Qt::SmoothTransformation));

  {
    QPainter painter(&pix);
    painter.drawPixmap(pix.width() - width, 0, thumb_pix);
  }

  KGameCanvasPixmap::setPixmap(pix);
}

void Sprite::removeThumb() {
  KGameCanvasPixmap::setPixmap(m_pixmap);
}

void Sprite::setPixmap(const QPixmap& pix) {
  m_pixmap = pix;
  KGameCanvasPixmap::setPixmap(pix);
}

void Sprite::setMovementAnimation(const shared_ptr<Animation>& animation) {
  m_movement_animation = animation;
}

weak_ptr<Animation> Sprite::movementAnimation() const {
  return m_movement_animation;
}
/*
void Sprite::setFadeAnimation(const shared_ptr<FadeAnimation>& animation) {
  m_fade_animation = animation;
}

weak_ptr<FadeAnimation> Sprite::fadeAnimation() const {
  return m_fade_animation;
}*/

void Sprite::setExplosionStep(float f) {
  m_explode_step = f;
  changed();
}

void Sprite::setupExplosion(Random& random) {
  if (m_explosion) {
    delete m_explosion;
    m_explosion = NULL;
  }
  m_explosion = createExplosion(random);
}

/*
 * this function generate many polygons that fill the image, + their random speeds
 *
 * Split points are some random point on the border (ordered accordin to the angle).
 * The generated polygons are created cutting from each split point to (according
 * to random's will) the center -or- a point in the mid way from the center to the
 * previous or next split point.
 */
SpriteExplosion* Sprite::createExplosion(Random& random) {
  SpriteExplosion* retv = new SpriteExplosion;
  int w = pixmap().width();
  int h = pixmap().height();
  float splits[40];
  float splits_r[40];
  float splits_i[40];
  int split_side[40];  // the side of the image, 0 to 3
  int split_start[40]; // -1 to 1
  QPoint split_point[40];
  QPoint split_start_point[40];
  int num_splits = 0;

  if(w==0 || h==0)
    return retv;

  /* generate few subsequent angles */
  Random::RealGenerator random_float = random.rand(0.4, 0.7);
  for(float s = 0; s < 2*M_PI-0.3; s += random_float()) {
    splits[num_splits] = s;
    splits_r[num_splits] = cos(s);
    splits_i[num_splits] = sin(s);
    num_splits++;
  }

  /* generate the random center and calculate the relative corners */
  random_float = random.rand(0.4, 0.6);
  QPoint center(int(w * random_float()), int(h * random_float()));
  QPoint corners[4] = { -center, QPoint(w,0)-center,
                  QPoint(w,h)-center, QPoint(0,h)-center };

  /* for each split angle, find the size of the rect that contains the split point */
  for(int i=0;i<num_splits;i++) {
    split_side[i] = -1;

    for(int j=0;j<4;j++) {
      int j1 = (j+1)%4;

      /* the imaginary part of the products (as complex numbers) must be >0 and <0 */
      if(corners[j].x()*splits_i[i]-corners[j].y()*splits_r[i] >= 0
          && corners[j1].x()*splits_i[i]-corners[j1].y()*splits_r[i] < 0 ) {

        split_side[i] = j;
        float fact = (corners[j].x()*corners[j1].y()-corners[j].y()*corners[j1].x())/
                  (splits_r[i]*(corners[j1].y()-corners[j].y())-
                    splits_i[i]*(corners[j1].x()-corners[j].x()));
        float x = splits_r[i]*fact;
        float y = splits_i[i]*fact;
        split_point[i] = QPoint(int(x+0.5), int(y+0.5));
        break;
      }
    }

    /* there should really be one :) */
    Q_ASSERT(split_side[i] != -1);
  }

  /* generate the split points connections. Some will be connected with the center (0),
     others will be connected to a mid-way point of the previous or next points (-1/+1) */
  Random::IntegerGenerator coin = random.rand(0, 1);
  Random::IntegerGenerator die3 = random.rand(0, 2);
  split_start[0] = 0;
  for(int i=1;i<num_splits;i++) {
    if(split_start[i-1]==1)
      split_start[i] = 0;
    else if(split_start[i-1]==-1)
      split_start[i] = coin();
    else
      split_start[i] = die3() - 1;
  }

  /* calculate such mid-way points */
  random_float = random.rand(0.1, 0.3);
  for(int i=0;i<num_splits;i++) {
    int r = (i+split_start[i]);
    r = r<0 ? num_splits-1 : r>=num_splits ? 0 : r;
    if(r == i)
      split_start_point[i] = QPoint();
    else
      split_start_point[i] = split_point[r] * random_float();
  }

  random_float = random.rand(0.5, 1.2);
  for(int i=0;i<num_splits;i++) {
    ExplosionFragment f;
    int i1 = (i+1 == num_splits) ? 0 : i+1;

    /* generate polygons, they have at most 6 sides */
    f.m_polygon[f.m_pcount++] = center + split_point[i];
    if(split_side[i] != split_side[i1])
      f.m_polygon[f.m_pcount++] = center + corners[split_side[i1]];
    f.m_polygon[f.m_pcount++] = center + split_point[i1];

    if(split_start[i1]==+1)
      f.m_polygon[f.m_pcount++] = center + split_start_point[i1];

    if(split_start[i1]==-1)
      f.m_polygon[f.m_pcount++] = center + split_start_point[i1];
    else if(split_start[i]==+1)
      f.m_polygon[f.m_pcount++] = center + split_start_point[i];
    else
      f.m_polygon[f.m_pcount++] = center;

    if(split_start[i]==-1)
      f.m_polygon[f.m_pcount++] = center + split_start_point[i];

    /* generate a random speed */
    f.m_speed = (split_point[i]+split_point[i1]) * random_float();

    retv->push_back(f);
  }

  return retv;
}

void Sprite::setRotation(float f) {
  m_rotation = f;
  changed();
}

void Sprite::setScale(float f) {
  m_scale = f;
  changed();
}

void Sprite::paint(QPainter* p) {
  QMatrix savem;

  /* if scale/rotate change the painter matrix */
  if(m_rotation != 0.0 || m_scale != 1.0) {
    QRectF rect = pixmap().rect();
    QPointF center(rect.width()*0.5, rect.height()*0.5);
    savem = p->matrix();
    p->translate(center+pos());
    p->rotate(m_rotation*180.0/M_PI);
    p->scale(m_scale, m_scale);
    p->translate(-center-pos());
  }

  if(m_explosion) {
    p->setPen(Qt::NoPen);
    p->setBrush(pixmap());
    p->translate(pos());

    for(int i=0;i<int(m_explosion->size());i++) {
      ExplosionFragment& f = (*m_explosion)[i];
      QPoint delta = f.m_speed*m_explode_step;

      p->translate(delta);
      p->drawConvexPolygon(f.m_polygon, f.m_pcount);
      p->translate(-delta);
    }
    p->translate(-pos());
  }
  else
    KGameCanvasPixmap::paint(p);

  if(m_rotation != 0.0 || m_scale != 1.0)
    p->setMatrix(savem);
}

QRect Sprite::rect() const {
  QRect retv;

  /* if exploding, set the rect to the bounding rect of the pieces */
  if(m_explosion) {
    int x1=99999, x2=-99999, y1=99999, y2=-99999;

    for(int i=0;i<int(m_explosion->size());i++) {
      ExplosionFragment& f = (*m_explosion)[i];
      QPoint delta = f.m_speed*m_explode_step;

      for(int j=0;j<f.m_pcount;j++) {
        x1 = std::min(x1, f.m_polygon[j].x()+delta.x());
        y1 = std::min(y1, f.m_polygon[j].y()+delta.y());
        x2 = std::max(x2, f.m_polygon[j].x()+delta.x());
        y2 = std::max(y2, f.m_polygon[j].y()+delta.y());
      }
    }

    retv = QRect(x1,y1,x2-x1,y2-y1).translated(pos());
  }
  else
    retv = KGameCanvasPixmap::rect();

  /* transform the rectangle as needed */
  if(m_rotation != 0.0 || m_scale != 1.0) {
    QRectF rect = pixmap().rect();
    QPointF center(rect.width()*0.5, rect.height()*0.5);
    QMatrix transf;
    transf.translate(+center.x()+pos().x(), +center.y()+pos().y());
    transf.rotate(m_rotation*180.0/M_PI);
    transf.scale(m_scale, m_scale);
    transf.translate(-center.x()-pos().x(), -center.y()-pos().y());
    return transf.mapRect( retv );
  }
  return retv;
}
