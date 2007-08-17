/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef TURNPOLICY_H
#define TURNPOLICY_H

#include <map>
#include <boost/shared_ptr.hpp>
#include "export.h"

namespace TurnPolicy {
  
  /**
    * An abstract class encapsulating the user ability
    * to move in a given turn.
    */
  class Abstract {
  public:
    virtual ~Abstract();
    virtual bool check() const = 0;
  };
  
  /**
    * Premove policy. Used to check whether premoving is allowed.
    * It is a <i>global</i> policy.
    */
  class Premove {
  public:
    virtual ~Premove();
    virtual bool check() const = 0;
  };
  
  class TAGUA_EXPORT Collection {
    typedef boost::shared_ptr<Abstract> PolicyPtr;
    typedef std::map<int, PolicyPtr> Policies;
    typedef boost::shared_ptr<Premove> PremovePolicyPtr;
    Policies m_policies;
        
    PremovePolicyPtr m_premove;
  public:
    bool operator()(int turn) const;
    void setPolicy(int turn, const PolicyPtr& policy);
    void setSimplePolicy(int turn, bool value);
    void clear();
    
    void setPremovePolicy(const PremovePolicyPtr& policy);
    bool premove() const;
  };
}

typedef TurnPolicy::Collection TurnTest;

#endif // TURNPOLICY_H
