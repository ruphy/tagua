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
#if defined(HAVE_X86_MMX) || defined(HAVE_X86_SSE)
  #include <kcpuinfo.h>
#endif



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

#ifdef HAVE_X86_SSE

#include <inttypes.h>
#include <xmmintrin.h>

union vec4i
{
  uint16_t i[8];
  __m128i v;
};

static inline void blur_sse_near(void* pixels, __m128i& state, __m128i alpha)
{
  uint64_t z1 = 0ULL;
  uint64_t z2 = 0ULL;
  uint64_t z3 = 0ULL;

  asm(
    "movq %[ppix], %[pixels]\n"
    "punpcklbw %[pixels], %[aux1]\n"           // unpack two pixels setting their bytes
                                               // as the most significant in the corr. word
    "psrlw $1, %[aux1]\n"                      // shift right by 1, i.e. shift the colour
                                               // bytes left by 7
    "psubw %[state], %[aux1]\n"                // - state
    "pmulhw %[alpha], %[aux1]\n"               // * alpha, and take the 16 most significant bits
    "psllw $1, %[aux1]\n"                      // shift left (we trade 1 bit for performance, here)
    "paddw %[aux1], %[state]\n"                // sum result to state
    "movdqa %[state], %[aux2]\n"                 // copy state to the aux2 register
    "psrlw $7, %[aux2]\n"                      // shift right by 7: this is the new pixel value
    "packuswb %[aux2], %[aux2]\n"              // pack pixels as 8 bits
    "movq %[aux2], %[ppix]\n"
  : [state] "+x"(state)
  , [ppix] "+m"(*(uint64_t*)pixels)
  , [aux1] "+x"(z1)
  , [aux2] "+x"(z2)
  , [pixels] "+x"(z3)
  : [alpha] "x"(alpha)
  );
}

static inline void blur_sse_sep(void* pixel1, void* pixel2, __m128i& state, __m128i alpha)
{
  uint64_t z1 = 0ULL;
  uint64_t z2 = 0ULL;
  uint64_t z3 = 0ULL;
  uint64_t z4 = 0ULL;

  asm(
    "movd %[ppix1], %[pixels]\n"             // load the first pixel
    "movd %[ppix2], %[tmp]\n"                // load the second pixel in [tmp]
    "pslldq $4, %[tmp]\n"                      // shift left the second pixel
    "paddd %[tmp], %[pixels]\n"                // now both pixel are packed in [pixels]

    "punpcklbw %[pixels], %[aux1]\n"           // unpack two pixels setting their bytes
                                               // as the most significant in the corr. word
    "psrlw $1, %[aux1]\n"                      // shift right by 1, i.e. shift the colour
                                               // bytes left by 7
    "psubw %[state], %[aux1]\n"                // - state
    "pmulhw %[alpha], %[aux1]\n"               // * alpha, and take the 16 most significant bits
    "psllw $1, %[aux1]\n"                      // shift left (we trade 1 bit for performance, here)
    "paddw %[aux1], %[state]\n"                // sum result to state
    "movdqa %[state], %[aux2]\n"                 // copy state to the aux2 register
    "psrlw $7, %[aux2]\n"                      // shift right by 7: this is the new pixel value
    "packuswb %[aux2], %[aux2]\n"              // pack pixels as 8 bits

    "movd %[aux2], %[ppix1]\n"
    "psrldq $4, %[aux2]\n"
    "movd %[aux2], %[ppix2]\n"
  : [state] "+x"(state)
  , [ppix1] "+m"(*(uint32_t*)pixel1)
  , [ppix2] "+m"(*(uint32_t*)pixel2)
  , [aux1] "+x"(z1)
  , [aux2] "+x"(z2)
  , [tmp]  "+x"(z3)
  , [pixels] "+x"(z4)
  : [alpha] "x"(alpha)
  );
}

static void expblur_sse( QImage &img, int radius )
{
  if(radius<1)
    return;

  /* Calculate the alpha such that 90% of 
     the kernel is within the radius.
     (Kernel extends to infinity) 
  */
  uint16_t alpha = (uint16_t)((1<<15)*(1.0f-expf(-2.3f/(radius+1.f))));

  vec4i a;
  QRgb *ptr = (QRgb *)img.bits();
  int h = img.height();
  int w = img.width();
  int hw = (img.height()-1)*img.width();
  for(int i=0;i<8;i++)
    a.i[i] = alpha;

  for(int row=0;row<h-1;row+=2)
  {
    vec4i z;
    uint8_t *cptr = (uint8_t*)(ptr+row*w);
    for(int i=0;i<4;i++)
      z.i[i] = cptr[i]<<7;
    for(int i=0;i<4;i++)
      z.i[4+i] = cptr[w*4+i]<<7;

    for(int index=1; index<w; index++)
      blur_sse_sep(&cptr[index*4], &cptr[(index+w)*4], z.v, a.v);

    for(int index=w-2; index>=0; index--)
      blur_sse_sep(&cptr[index*4], &cptr[(index+w)*4] , z.v, a.v);
  }

  if(h & 1)
  {
    vec4i z;
    int dummy;
    uint8_t *cptr = (uint8_t*)(ptr+(h-1)*w);
    for(int i=0;i<4;i++)
      z.i[i] = cptr[i]<<7;

    for(int index=1; index<w; index++)
      blur_sse_sep(&cptr[index*4], &dummy, z.v, a.v);

    for(int index=w-2; index>=0; index--)
      blur_sse_sep(&cptr[index*4], &dummy, z.v, a.v);
  }

  for(int col=0;col<w-1;col+=2)
  {
    vec4i z;
    uint8_t *cptr = (uint8_t*)(ptr+col);

    for(int i=0;i<8;i++)
      z.i[i] = cptr[i]<<7;

    for(int index=w; index<hw; index+=w)
      blur_sse_near(&cptr[index*4], z.v, a.v);

    for(int index=hw-2*w; index>=0; index-=w)
      blur_sse_near(&cptr[index*4], z.v, a.v);
  }

  if(w & 1)
  {
    vec4i z;
    int dummy;
    uint8_t *cptr = (uint8_t*)(ptr+w-1);

    for(int i=0;i<4;i++)
      z.i[i] = cptr[i]<<7;

    for(int index=w; index<hw; index+=w)
      blur_sse_sep(&cptr[index*4], &dummy, z.v, a.v);

    for(int index=hw-w; index>=0; index-=w)
      blur_sse_sep(&cptr[index*4], &dummy, z.v, a.v);
  }

  return;
}
#endif //HAVE_X86_SSE


#ifdef HAVE_X86_MMX

#include <inttypes.h>
#include <mmintrin.h>

union vec4s
{
  uint16_t i[4];
  __m64 v;
};

static inline void blur_mmx(void *px, __m64& v, __m64& alpha)
{
  uint64_t z1 = 0ULL;
  uint64_t z2 = 0ULL;
  asm(
    "movd %[pixel], %[t1]\n"
    "punpcklbw %[t1], %[t2]\n"
    "psrlw $1, %[t2]\n"
    "psubw %[accum], %[t2]\n"
    "pmulhw %[alpha], %[t2]\n"
    "psllw $1, %[t2]\n"
    "paddw %[t2], %[accum]\n"
    "movq %[accum], %[t1]\n"
    "psrlw $7, %[t1]\n"
       // "pand %[mask], %[t1]\n"
    "packuswb %[t1], %[t1]\n"
    "movd %[t1], %[pixel]\n"
    :  [pixel] "+m"(*(uint32_t*)px)
     , [accum] "+y"(v)
     , [t1] "+y"(z1)
     , [t2] "+y"(z2)
    :  [alpha] "y"(alpha)
     // , [mask] "y"(0x00ff00ff00ff00ffULL)
  );
}

static void expblur_mmx( QImage &img, int radius )
{
  if(radius<1)
    return;

  /* Calculate the alpha such that 90% of 
     the kernel is within the radius.
     (Kernel extends to infinity) 
  */
  uint16_t alpha = (uint16_t)((1<<15)*(1.0f-expf(-2.3f/(radius+1.f))));

  vec4s a;
  QRgb *ptr = (QRgb *)img.bits();
  int h = img.height();
  int w = img.width();
  int hw = (img.height()-1)*img.width();
  for(int i=0;i<4;i++)
    a.i[i] = alpha;

  for(int row=0;row<h;row++)
  {
    vec4s z;
    uint8_t *cptr = (uint8_t*)(ptr+row*w);
    for(int i=0;i<4;i++)
      z.i[i] = cptr[i]<<7;

    for(int index=1; index<w; index++)
      blur_mmx(&cptr[index*4], z.v, a.v);

    for(int index=w-2; index>=0; index--)
      blur_mmx(&cptr[index*4], z.v, a.v);
  }

  for(int col=0;col<w;col++)
  {
    vec4s z;
    uint8_t *cptr = (uint8_t*)(ptr+col);

    for(int i=0;i<4;i++)
      z.i[i] = cptr[i]<<7;

    for(int index=w; index<hw; index+=w)
      blur_mmx(&cptr[index*4], z.v, a.v);

    for(int index=hw-w; index>=0; index-=w)
      blur_mmx(&cptr[index*4], z.v, a.v);
  }

  asm("emms");
  return;
}
#endif //HAVE_X86_MMX




namespace ImageEffects {

void expBlur(QImage& img, int radius) {
#ifdef HAVE_X86_SSE
  if(KCPUInfo::haveExtension( KCPUInfo::IntelSSE ) )
    return expblur_sse(img, radius);
#endif
#ifdef HAVE_X86_MMX
  if(KCPUInfo::haveExtension( KCPUInfo::IntelMMX ) )
    return expblur_mmx(img, radius);
#endif
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
