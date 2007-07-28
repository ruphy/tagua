/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "turnpolicy.h"

namespace TurnPolicy {

  Abstract::~Abstract() { }

  boost::shared_ptr<Always> always() { 
    return boost::shared_ptr<Always>(new Always); 
  }
  
  boost::shared_ptr<Never> never() { 
    return boost::shared_ptr<Never>(new Never); 
  }

  Premove::~Premove() { }

  bool Collection::operator()(int turn) const {
    Policies::const_iterator it = m_policies.find(turn);
    if (it == m_policies.end()) {
      return false;
    }
    else {
      return it->second->check();
    }
  }

  void Collection::setPolicy(int turn, const PolicyPtr& policy) {
    m_policies[turn] = policy;
  }
  
  void Collection::setSimplePolicy(int turn, bool value) {
    if (value)
      setPolicy(turn, always());
    else
      setPolicy(turn, never());
  }
  
  void Collection::clear() {
    m_policies.clear();
    m_premove.reset();
  }
  
  bool Collection::premove() const {
    return m_premove ? m_premove->check() : false;
  }
  
  void Collection::setPremovePolicy(const PremovePolicyPtr& policy) {
    m_premove = policy;
  }

}


