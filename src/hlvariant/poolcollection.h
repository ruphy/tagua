#ifndef HLVARIANT__POOLCOLLECTION_H
#define HLVARIANT__POOLCOLLECTION_H

namespace HLVariant {

template <typename _Pool>
class PoolCollection {
public:
  typedef _Pool Pool;
private:
  typedef typename Pool::Piece Piece;
  typedef typename Piece::Color Color;
  typedef std::map<Color, Pool> Pools;
  
  Pools m_pools;
public:
  virtual ~PoolCollection();
  
  virtual bool operator==(const PoolCollection<Pool>& other) const;
  virtual bool operator!=(const PoolCollection<Pool>& other) const;
  
  virtual Pool& pool(Color player);
  virtual const Pool& pool(Color player) const;
};

// IMPLEMENTATION

template <typename Pool>
PoolCollection<Pool>::~PoolCollection() { }

template <typename Pool>
bool PoolCollection<Pool>::operator==(const PoolCollection<Pool>& other) const {
  typename Pools::const_iterator i = m_pools.begin();
  typename Pools::const_iterator j = other.m_pools.begin();
  
  while (i != m_pools.end() && j != other.m_pools.end()) {
    if (i->first < j->first) {
      if (!i->second.empty())
        return false;
      else
        ++i;
    }
    else if (i->first > j->first) {
      if (!j->second.empty())
        return false;
      else
        ++j;
    }
    else {
      // same key, compare values
      if (i->second != j->second)
        return false;
      else {
        ++i;
        ++j;
      }
    }
  }
  
  // check tail
  while (i != m_pools.end()) {
    if (!i->second.empty())
      return false;
    ++i;
  }
  while (j != other.m_pools.end()) {
    if (!j->second.empty())
      return false;
    ++j;
  }
  
  return true;
}

template <typename Pool>
bool PoolCollection<Pool>::operator!=(const PoolCollection<Pool>& other) const {
  return !((*this) == other);
}

template <typename Pool>
Pool& PoolCollection<Pool>::pool(Color player) {
  // return pool if it exists
  typename Pools::iterator it = m_pools.find(player);
  if (it != m_pools.end()) {
    return it->second;
  }
  
  // if not, create it  
  m_pools.insert(std::make_pair(player, Pool(player)));
  return m_pools.find(player)->second;
}

template <typename Pool>
const Pool& PoolCollection<Pool>::pool(Color player) const {
  // here we use const cast, because our semantics of PoolCollection
  // considers an empty pool the same thing as no pool.
  return const_cast<PoolCollection*>(this)->pool(player);
}

}

#endif // HLVARIANT__POOLCOLLECTION_H

