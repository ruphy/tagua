/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef CLICKABLECANVAS_H
#define CLICKABLECANVAS_H

#include "canvas/group.h"

class ClickableCanvas : public Canvas::Group {
public:
  ClickableCanvas(Abstract* canvas = NULL) : Canvas::Group(canvas) { }
  virtual void onMousePress(const QPoint& point, int button) = 0;
  virtual void onMouseMove(const QPoint& point, int button) = 0;
  virtual void onMouseRelease(const QPoint& point, int button) = 0;
  virtual void onMouseEnter() {}
  virtual void onMouseLeave() {}
};

#endif // CLICKABLECANVAS_H
