/*
  Copyright (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Part of the code been contributed by Jani Huhtanen.
  Copyright (c) 2006 Jani Huhtanen.
*/


#include <cmath>
#include <QPainter>
#include "imageeffects.h"
#include <kcpuinfo.h>


template<int aprec, int zprec>
static inline void blurinner(unsigned char *bptr, int &zR,
                             int &zG, int &zB, int &zA, int alpha);

template<int aprec,int zprec>
static inline void blurrow(QImage & im, int line, int alpha);

template<int aprec, int zprec>
static inline void blurcol(QImage & im, int col, int alpha);

/*
 *  expblur(QImage &img, int radius)
 *
 *  In-place blur of image 'img' with kernel
 *  of approximate radius 'radius'.
 *
 *  Blurs with two sided exponential impulse
 *  response.
 *
 *  aprec = precision of alpha parameter
 *  in fixed-point format 0.aprec
 *
 *  zprec = precision of state parameters
 *  zR,zG,zB and zA in fp format 8.zprec
 */
template<int aprec,int zprec>
static void expblur(QImage &img, int radius )
{
    if (radius < 1)
        return;

    /* Calculate the alpha such that 90% of
       the kernel is within the radius.
       (Kernel extends to infinity)
    */
    int alpha = (int)((1<<aprec)*(1.0f-expf(-2.3f/(radius+1.f))));

    for(int row=0;row<img.height();row++) {
        blurrow<aprec,zprec>(img,row,alpha);
    }

    for(int col=0;col<img.width();col++) {
        blurcol<aprec,zprec>(img,col,alpha);
    }
    return;
}

template<int aprec, int zprec>
static inline void blurinner(unsigned char *bptr,
                             int &zR, int &zG, int &zB, int &zA, int alpha)
{
    int R,G,B,A;
    R = *bptr;
    G = *(bptr+1);
    B = *(bptr+2);
    A = *(bptr+3);

    zR += (alpha * ((R<<zprec)-zR))>>aprec;
    zG += (alpha * ((G<<zprec)-zG))>>aprec;
    zB += (alpha * ((B<<zprec)-zB))>>aprec;
    zA += (alpha * ((A<<zprec)-zA))>>aprec;

    *bptr =     zR>>zprec;
    *(bptr+1) = zG>>zprec;
    *(bptr+2) = zB>>zprec;
    *(bptr+3) = zA>>zprec;
}

template<int aprec,int zprec>
static inline void blurrow( QImage & im, int line, int alpha)
{
    int zR,zG,zB,zA;

    QRgb *ptr = (QRgb *)im.scanLine(line);

    zR = *((unsigned char *)ptr    )<<zprec;
    zG = *((unsigned char *)ptr + 1)<<zprec;
    zB = *((unsigned char *)ptr + 2)<<zprec;
    zA = *((unsigned char *)ptr + 3)<<zprec;

    for(int index=1; index<im.width(); index++) {
        blurinner<aprec,zprec>((unsigned char *)&ptr[index],
                               zR, zG, zB, zA, alpha);
    }
    for(int index=im.width()-2; index>=0; index--) {
        blurinner<aprec,zprec>((unsigned char *)&ptr[index],
                               zR, zG, zB, zA, alpha);
    }


}

template<int aprec, int zprec>
static inline void blurcol(QImage & im, int col, int alpha)
{
    int zR,zG,zB,zA;

    QRgb *ptr = (QRgb *)im.bits();
    ptr+=col;

    zR = *((unsigned char *)ptr    )<<zprec;
    zG = *((unsigned char *)ptr + 1)<<zprec;
    zB = *((unsigned char *)ptr + 2)<<zprec;
    zA = *((unsigned char *)ptr + 3)<<zprec;

    for(int index=im.width(); index<(im.height()-1)*im.width();
        index+=im.width()) {
        blurinner<aprec,zprec>((unsigned char *)&ptr[index],
                               zR, zG, zB, zA, alpha);
    }

    for(int index=(im.height()-2)*im.width(); index>=0;
        index-=im.width()) {
        blurinner<aprec,zprec>((unsigned char *)&ptr[index],
                               zR, zG, zB, zA, alpha);
    }

}

namespace ImageEffects {


#ifdef HAVE_X86_SSE
  void expblur_sse( QImage &img, int radius );
#endif //HAVE_X86_SSE


#ifdef HAVE_X86_MMX
  void expblur_mmx( QImage &img, int radius );
#endif //HAVE_X86_MMX



void expBlur(QImage& img, int radius) {

//KCPUInfo:: returns false on x86_64, and x86_64 always have sse/mmx
#ifdef HAVE_X86_SSE
  #ifndef __x86_64__
    if(KCPUInfo::haveExtension( KCPUInfo::IntelSSE ) )
  #endif //__x86_64__
      return expblur_sse(img, radius);
#endif //HAVE_X86_SSE

#ifdef HAVE_X86_MMX
  #ifndef __x86_64__
    if(KCPUInfo::haveExtension( KCPUInfo::IntelMMX ) )
  #endif //__x86_64__
      return expblur_mmx(img, radius);
#endif //HAVE_X86_MMX

  return expblur<15,7>(img, radius);
}

QImage addShadow(const QImage& image, int r, QColor color,
                         int offx, int offy, int growx, int growy) {
  QPainter p;
  QImage retv(image.width()+growx, image.height()+growy, QImage::Format_ARGB32_Premultiplied);
  int dx = (growx-offx)/2, dy = (growy-offy)/2;

  p.begin(&retv);
  p.setCompositionMode(QPainter::CompositionMode_Source);
  p.fillRect(0,0,retv.width(), retv.height(), QColor(0,0,0,0));
  p.fillRect(dx+offx, dy+offy, image.width(), image.height(), color);
  p.setCompositionMode(QPainter::CompositionMode_DestinationAtop );
  p.drawImage(dx+offx, dy+offy, image);
  p.end();

  expBlur(retv, r);

  p.begin(&retv);
  p.drawImage(dx, dy, image);
  p.end();

  return retv;
}

QImage growBorder(const QImage& image) {
  int w = image.width();
  int h = image.height();
  QPainter p;
  QImage retv(w+2, h+2, QImage::Format_ARGB32_Premultiplied);

  p.begin(&retv);
  p.setCompositionMode(QPainter::CompositionMode_Source);
  p.drawImage(0, 0, image, 0, 0, 1, 1);
  p.drawImage(w+1, 0, image, w-1, 0, 1, 1);
  p.drawImage(0, h+1, image, 0, h-1, 1, 1);
  p.drawImage(w+1, h+1, image, w-1, h-1, 1, 1);
  p.drawImage(1, 0, image, 0, 0, w, 1);
  p.drawImage(1, h+1, image, 0, h-1, w, 1);
  p.drawImage(0, 1, image, 0, 0, 1, h);
  p.drawImage(w+1, 1, image, w-1, 0, 1, h);
  p.drawImage(1, 1, image);
  p.end();

  return retv;
}

struct Line {
  int y, x1, x2;
  Line(){}
  Line(int _y, int _x1, int _x2)
    : y(_y), x1(_x1), x2(_x2) {}
};

void floodFill(QImage& image, QPoint point, QColor color,
                bool invade_border, std::vector<QPoint>* border) {

  int* ptr = (int*)image.bits();
  int h = image.height();
  int w = image.width();
  int newcol = color.rgba();
  int oldcol = ptr[point.x()+point.y()*w];
  std::vector<Line> lines;

  {
  Line l(point.y(), point.x(), point.x()+1);
  int *scanline = ptr+point.y()*w;
  scanline[l.x1] = newcol;
  while(l.x1 > 0 && scanline[l.x1-1] == oldcol)
    scanline[--l.x1] = newcol;
  while(l.x2 < w && scanline[l.x2] == oldcol)
    scanline[l.x2++] = newcol;
  lines.push_back(l);
  }

  while(!lines.empty()) {
    Line ll = lines[lines.size()-1];
    lines.pop_back();

    if(ll.x1>0) {
      if(invade_border)
        ptr[ll.y*w + ll.x1 - 1] = newcol;
      if(border)
        border->push_back(QPoint(ll.x1-1, ll.y));
    }
    if(ll.x2<w) {
      if(invade_border)
        ptr[ll.y*w + ll.x2] = newcol;
      if(border)
        border->push_back(QPoint(ll.x2, ll.y));
    }

    for(int d=-1; d<=1; d+=2)
    if( (d == -1) ? (ll.y > 0) : (ll.y < h-1) ) {
      int *scanline = ptr + (ll.y+d)*w;

      for(int i=ll.x1;i<ll.x2;i++){
        if(scanline[i]==oldcol) {
          Line l(ll.y+d, i, i+1);

          scanline[l.x1] = newcol;
          while(l.x1 > 0 && scanline[l.x1-1] == oldcol)
            scanline[--l.x1] = newcol;
          while(l.x2 < w && scanline[l.x2] == oldcol)
            scanline[l.x2++] = newcol;
          lines.push_back(l);
          i = l.x2;
        }
        if(i<ll.x2 && scanline[i]!=newcol) {
          if(invade_border)
            scanline[i]=newcol;
          if(border)
            border->push_back(QPoint(i, ll.y+d));
        }
      }
    }
  }
}

void floodFillBlueThreshold(QImage& image, QPoint point, QColor color, unsigned int thresh,
                          bool invade_border, std::vector<QPoint>* border) {

  unsigned int* ptr = (unsigned int*)image.bits();
  int h = image.height();
  int w = image.width();
  unsigned int newcol = color.rgba();
  std::vector<Line> lines;

  #define TEST(x) ((((x) & 0xff) < thresh) && ((x) != newcol))
  {
  Line l(point.y(), point.x(), point.x()+1);
  unsigned int *scanline = ptr+point.y()*w;
  scanline[l.x1] = newcol;
  while(l.x1 > 0 && TEST(scanline[l.x1-1]))
    scanline[--l.x1] = newcol;
  while(l.x2 < w && TEST(scanline[l.x2]))
    scanline[l.x2++] = newcol;
  lines.push_back(l);
  }

  while(!lines.empty()) {
    Line ll = lines[lines.size()-1];
    lines.pop_back();

    if(ll.x1>0) {
      if(invade_border)
        ptr[ll.y*w + ll.x1 - 1] = newcol;
      if(border)
        border->push_back(QPoint(ll.x1-1, ll.y));
    }
    if(ll.x2<w) {
      if(invade_border)
        ptr[ll.y*w + ll.x2] = newcol;
      if(border)
        border->push_back(QPoint(ll.x2, ll.y));
    }

    for(int d=-1; d<=1; d+=2)
    if( (d == -1) ? (ll.y > 0) : (ll.y < h-1) ) {
      unsigned int *scanline = ptr + (ll.y+d)*w;

      for(int i=ll.x1;i<ll.x2;i++){
        if(TEST(scanline[i])) {
          Line l(ll.y+d, i, i+1);

          scanline[l.x1] = newcol;
          while(l.x1 > 0 && TEST(scanline[l.x1-1]))
            scanline[--l.x1] = newcol;
          while(l.x2 < w && TEST(scanline[l.x2]))
            scanline[l.x2++] = newcol;
          lines.push_back(l);
          i = l.x2;
        }
        if(i<ll.x2 && scanline[i]!=newcol) {
          if(invade_border)
            scanline[i]=newcol;
          if(border)
            border->push_back(QPoint(i, ll.y+d));
        }
      }
    }
  }
}

}
