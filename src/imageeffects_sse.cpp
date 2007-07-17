/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <math.h>
#include <inttypes.h>
#include <xmmintrin.h>
#include <QImage>


namespace ImageEffects {

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

void expblur_sse( QImage &img, int radius )
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

} //end namespace ImageEffects
