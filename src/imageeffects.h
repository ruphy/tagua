/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef IMAGEEFFECTS_H
#define IMAGEEFFECTS_H

#include <QImage>
#include <QPoint>
#include <vector>

namespace ImageEffects {
  void expBlur(QImage& img, int radius);
  QImage addShadow(const QImage& image, int radius, QColor color,
                                int offx, int offy, int growx, int growy);
  void floodFill(QImage& image, QPoint point, QColor color,
            bool invade_border = false, std::vector<QPoint>* border = NULL);
  void floodFillBlueThreshold(QImage& image, QPoint point, QColor color, unsigned int thresh,
            bool invade_border = false, std::vector<QPoint>* border = NULL);
  QImage growBorder(const QImage& image);
} //end namespace ImageEffects

#endif //IMAGEEFFECTS_H
