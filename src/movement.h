/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVEMENT_H
#define MOVEMENT_H

#include <QPoint>
#include <iostream>

/**
  * Abstract class representing a parameterization of a curve
  * in a plane and a rotation angle.
  * The parameter @a t for both the position and the rotation
  * is assumed to lie in the closed interval [0,1].
  */
class Movement {
public:
  virtual ~Movement() { }
  virtual QPoint pos(double t) const = 0;
  virtual double rotation(double /*t*/) const { return 0.0; }

  static double sigmoid(double t);
};

/**
  * A straight path, no rotation.
  */
class LinearMovement : public Movement {
  QPoint m_from;
  QPoint m_velocity;
public:
  /// create a path from @a from to @a to.
  LinearMovement(const QPoint& from, const QPoint& to, bool /*rotate*/);

  virtual QPoint pos(double t) const;
};

/**
  * An L-shaped path. First along the shortest side, then along the other.
  * Use an exponential parameterization for the longest side.
  */
class LMovement : public Movement {
  QPoint m_from;
  QPoint m_velocity;
  bool m_rotate;
  double (*m_x)(double);
  double (*m_y)(double);

  static double linear(double t) { return t; }
  static double nonlinear(double t);
public:
  /// create an L-path from @a from to @a to.
  LMovement(const QPoint& from, const QPoint& to, bool rotate = false);

  virtual QPoint pos(double t) const;
  virtual double rotation(double t) const;
};

/**
  * Movement mixin: apply to a Movement to get a sigmoidal
  * variant of it.
  */
template <typename Mov>
class SigmoidalMovement : public Mov {
public:
  SigmoidalMovement(const QPoint& from, const QPoint& to, bool rotate = false)
  : Mov(from, to, rotate) { }

  virtual QPoint pos(double t) const {
    return Mov::pos(Movement::sigmoid(t));
  }
};

//BEGIN MovementFactory

//NOTE for Paolo: WTF?
class AbstractMovementFactory {
public:
  virtual ~AbstractMovementFactory() { }
  virtual Movement* create(const QPoint& from, const QPoint& to) const = 0;
};

/**
  * MovementFactory is used to pass around Movement information
  * whenever movement parameters @a from and @a to are not yet known.
  */
template <typename Mov>
class MovementFactory : public AbstractMovementFactory {
public:
  virtual Mov* create(const QPoint& from, const QPoint& to) const {
    return new SigmoidalMovement<Mov>(from, to);
  }
};

//END MovementFactory

#endif // MOVEMENT_H
