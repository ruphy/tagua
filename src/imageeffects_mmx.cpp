/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <math.h>
#include <inttypes.h>
#include <mmintrin.h>
#include <QImage>


namespace ImageEffects {

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

void expblur_mmx( QImage &img, int radius )
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

} //end namespace ImageEffects
