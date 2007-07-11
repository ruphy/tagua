/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef LOADER__CONTEXT_H
#define LOADER__CONTEXT_H

#include <map>
#include <set>
#include <boost/any.hpp>
#include <QString>

namespace Loader {

/**
  * @class Context <loader/context.h>
  * @brief A resource loading context
  *
  * This class offers a set of references in a global cache, to access
  * a global cache remembering which elements of the cache are being used.
  *
  */
class Context {

public:
  class Key {
  public:
    QString m_name;
    const char *m_type;

    Key(const QString& name, const char *type)
      : m_name(name)
      , m_type(type) {
    }

    bool operator<(const Key& ref) const {
      return (m_name < ref.m_name) ||
        ((m_name == ref.m_name) && (strcmp(m_type, ref.m_type) < 0));
    }
  };

  class Resource {
  public:
    int m_ref_count;
    boost::any m_data;

    Resource()
      : m_ref_count(1) {}

    template<typename T>
    Resource(const T& data)
      : m_ref_count(1)
      , m_data(data) {
    }
  };

  typedef std::map<Key, Resource> Cache;
  typedef std::set<Key> KeySet;

  static Cache s_cache;
  KeySet m_references;

public:
  /** Destructor (flushes the context) */
  ~Context();

  /** Flushes all the references in this context. */
  void flush();

  /**
    * Gets a resource by name.
    * @param name The resource name
    * @return The resource pointer, or null if not found.
    */
  template<typename T>
  T* get(const QString& name) {
    Key key(name, typeid(T).name());
    Cache::iterator it = s_cache.find(key);

    if(it == s_cache.end())
      return NULL;
    if(!m_references.count(key)) {
      it->second.m_ref_count++;
      m_references.insert(key);
    }
    return boost::any_cast<T>(&it->second.m_data);
  }

  /**
    * Puts a new resource in the global cache, and makes it
    * referenced by this context.
    * @param name The resource name
    */
  template<typename T>
  void put(const QString& name, const T& data) {
    Key key(name, typeid(T).name());
    Q_ASSERT(!s_cache.count(key));
    Q_ASSERT(!m_references.count(key));

    s_cache[key] = Resource(data);
    m_references.insert(key);
  }

  /**
    * For debugging
    */
  static void dump();
};

} //end namespace Loader

#endif //LOADER__CONTEXT_H
