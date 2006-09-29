/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "movement.h"
#include "common.h"
#include <cmath>

using std::abs;
#define QUINTIC_SIGMOID

double Movement::sigmoid(double t) {
#ifdef CUBIC_SIGMOID
  return -2 * pow(t - 0.5, 3) + 1.5 * t - 0.25;
#else
  return 6 * pow(t - 0.5, 5) - 5 * pow(t, 3) + 7.5 * t * t - 15 * 0.125 * t + 3.0 / 16;
#endif
}

LinearMovement::LinearMovement(const QPoint& from, const QPoint& to, bool /*rotate*/)
: m_from(from)
, m_velocity(to - from) { }

QPoint LinearMovement::pos(double t) const {
  return m_from + m_velocity * t;
}

LMovement::LMovement(const QPoint& from, const QPoint& to, bool rotate)
: m_from(from)
, m_velocity(to - from)
, m_rotate(rotate) {
  if (abs(static_cast<double>(m_velocity.x())) >=
      abs(static_cast<double>(m_velocity.y()))) {
    m_x = LMovement::nonlinear;
    m_y = LMovement::linear;
  }
  else {
    m_y = LMovement::nonlinear;
    m_x = LMovement::linear;
  }
}

double LMovement::nonlinear(double t) {
  return (exp( (double)(3 * t) ) - 1) / (exp( (double)3 ) - 1);
}

QPoint LMovement::pos(double t) const {
  return m_from + QPoint(
    static_cast<int>(m_velocity.x() * m_x(t)),
    static_cast<int>(m_velocity.y() * m_y(t)));
}

double LMovement::rotation(double t) const {
  if(!m_rotate)
    return 0.0;

  double r = (1.0-cos(t*M_PI))*M_PI;
  return m_velocity.x() >= 0 ? r : -r;
}

