/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef RANDOM_H
#define RANDOM_H

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/variate_generator.hpp>
#include <boost/random/uniform_smallint.hpp>
#include <boost/random/uniform_real.hpp>

class Random {
public:
  typedef boost::mt19937 RandomGenerator;
  typedef boost::uniform_smallint<> IntegerDistribution;
  typedef boost::uniform_real<float> RealDistribution;
  typedef boost::variate_generator<RandomGenerator*, IntegerDistribution> IntegerGenerator;
  typedef boost::variate_generator<RandomGenerator*, RealDistribution> RealGenerator;
private:
  RandomGenerator m_generator;
  
  Random(); // singleton
  static Random* m_instance;
public:
  IntegerGenerator rand(int min, int max);
  RealGenerator rand(float min, float max);
  RealGenerator rand(double min, double max);
  
  static Random& instance();
};

#endif // RANDOM_H
