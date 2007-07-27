#ifndef EXPORT_H
#define EXPORT_H

#include <kdemacros.h>

#ifdef TAGUA_DEBUG
  #ifdef MAKE_TAGUAPRIVATE_LIB
    #define TAGUA_EXPORT KDE_EXPORT
  #else
    #define TAGUA_EXPORT KDE_IMPORT
  #endif // MAKE_TAGUAPRIVATE_LIB
#else // TAGUA_DEBUG
  #define TAGUA_EXPORT
#endif // TAGUA_DEBUG

#endif // EXPORT_H
