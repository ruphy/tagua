/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

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
