/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef BOARDSPRITE_H
#define BOARDSPRITE_H

#include <QPixmap>
#include "canvas/canvas.h"
#include "point.h"

class BoardSprite : public Canvas::Pixmap {
  public:
    BoardSprite(QObject* /*parent*/, const QPixmap& pix, Canvas::Abstract* canvas)
      : Canvas::Pixmap(pix, canvas) {
    }
};

#endif // BOARDSPRITE_H
