/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "mainanimation.h"

using namespace boost;

MainAnimation::MainAnimation(double speed)
: AnimationGroup(true)
, m_translate(0)
, m_speed(speed) {
  m_active = true;
  m_time.start();
  m_timer.start(20);

  connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

void MainAnimation::addAnimation(const shared_ptr<Animation>& a) {
  AnimationGroup::addPostAnimation(a);
  tick();
}

void MainAnimation::stop() {
  AnimationGroup::stop();
}

void MainAnimation::tick() {
  animationAdvance( int(m_time.elapsed()*m_speed) + m_translate);
}

void MainAnimation::setSpeed(double speed) {
  int e = m_time.elapsed();
  m_translate = int((e*m_speed + m_translate) - e*speed);
  m_speed = speed;
}

void MainAnimation::setDelay(int delay) {
  m_timer.setInterval(delay);
}

#include "mainanimation.moc"
