/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <iostream>
#include <iomanip>
#include "common.h"
#include "loader/context.h"

namespace Loader {


Context::Cache Context::s_cache;


void Context::flush() {
  for(KeySet::iterator i = m_references.begin();
        i != m_references.end(); ) {
    Cache::iterator cit = s_cache.find(*i);
    Q_ASSERT(cit != s_cache.end());
    Q_ASSERT(cit->second.m_ref_count > 0);

    m_references.erase(i++);

    if( !--cit->second.m_ref_count )
      s_cache.erase(cit);
  }
}


Context::~Context() {
  flush();
  Q_ASSERT(m_references.empty());
}

void Context::dump() {
    for(Cache::iterator c = s_cache.begin(); c != s_cache.end(); ++c)
      std::cout << "*"<< std::setiosflags(std::ios::left)
                << std::setw(2) << c->second.m_ref_count
                << std::setiosflags(std::ios::left)
                << std::setw(50) << prettyTypeName(c->first.m_type)
              << " \"" << c->first.m_name << "\"" << std::endl;
}

} //end namespace Loader

