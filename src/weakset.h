/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef WEAK_SET
#define WEAK_SET

#include <boost/weak_ptr.hpp>
#include <list>

#ifdef WEAK_SET_DEBUG
  #include <iostream>
  using namespace std;
#endif // WEAK_SET_DEBUG

/**
  * @brief An implementation of a set of weak references.
  * 
  * weak_set is a set of weak_pointers which are automatically collected
  * and removed when the pointed object is destroyed.
  */
template <typename T>
class weak_set {  
  typedef std::list<boost::weak_ptr<T> > inner_set;
  typedef typename inner_set::iterator inner_iterator;
  friend class iterator;
  mutable inner_set m_set;
public:
  class iterator {
    friend class weak_set<T>;
    mutable inner_iterator m_it;
    boost::shared_ptr<T> obj; // this prevents the stored object from being collected
    weak_set<T>& m_parent;
    
    iterator(const inner_iterator& it, weak_set<T>& parent)
    : m_it(it)
    , m_parent(parent) {
      skip_garbage();
    }
    
    void skip_garbage() {
      #ifdef WEAK_SET_DEBUG
      cout << "skipping garbage" << endl;
      uint c = 0;
      #endif // WEAK_SET_DEBUG
      while (true) {
        if (m_it == m_parent.m_set.end()) break;
        boost::weak_ptr<T> wp = *m_it;
        if (obj = wp.lock()) break;

//      while (m_it != m_parent.m_set.end() && !(obj = (*m_it).lock())) {
        #ifdef WEAK_SET_DEBUG
        cout << "iteration " << c++ << endl;
        #endif // WEAK_SET_DEBUG

        // the current reference is dangling:
        // remove it from the list and get the next one
        #ifdef WEAK_SET_DEBUG
        cout << "dangling reference! removing it" << endl;
        #endif // WEAK_SET_DEBUG
        m_it = m_parent.m_set.erase(m_it);
      }
      // at this point either 
      // m_it < m_parent.m_set.end() _and_ obj is a valid shared_ptr
      // or 
      // m_it == m_parent.m_set.end()
      #ifdef WEAK_SET_DEBUG
      if (obj)
        cout << "no iteration done: uses = " << obj.use_count() << ", obj = " << *obj << endl;
      else
        cout << "we're at the end" << endl;
      #endif // WEAK_SET_DEBUG
    }
  public:
    /**
      * Check two iterators for equality.
      */
    bool operator==(const iterator& other) { return m_it == other.m_it; }
    
    /**
      * Equivalent to !operator==.
      */
    bool operator!=(const iterator& other) { return !operator==(other); }
    
    /**
      * Pre-increment operator. Modifity the iterator so
      * that it points to the next valid object.
      * @return The incremented iterator.
      */
    iterator& operator++() {
      #ifdef WEAK_SET_DEBUG
      cout << "incrementing" << endl;
      #endif // WEAK_SET_DEBUG
      ++m_it;
      obj.reset();
      skip_garbage();
      #ifdef WEAK_SET_DEBUG
      cout << "done incrementing" << endl;
      #endif // WEAK_SET_DEBUG
      return *this;
    }
    
    /**
      * Post-increment operator. Modifity the iterator so
      * that it points to the next valid object.
      * @return The iterator before being incremented.
      */
//    iterator operator++(int) { return iterator(m_it++, m_parent); }
    
    /**
      * @return a pointer to the stored object.
      * @note This function is guaranteed to return a valid
      * non-null pointer if the iterator is valid
      * and less than the past-the-end iterator.
      */
    T* get() { return obj.get(); }
    
    /**
      * @return a pointer to the stored object.
      * @sa get
      */
    T* operator->() { return get(); }
    
    /**
      * @return a reference to the stored object.
      * @sa get
      */
    T& operator*() { return *get(); }
  };

  /**
    * Add an object to the container.
    * @return an iterator referring to the newly inserted object.
    * @note No iterator is invalidated by an insert operation.
    */
  iterator insert(const boost::shared_ptr<T>& x) {
    return iterator(m_set.insert(m_set.end(), x), *this);
  }
  
  /**
    * @return an iterator pointing to the first element of
    *         the container, which can be used for iterating.
    */
  iterator begin() {
    return iterator(m_set.begin(), *this);
  }
  
  /**
    * @return a past-the-end iterator.
    */
  iterator end() {
    return iterator(m_set.end(), *this);
  }
};

#endif // WEAK_SET

