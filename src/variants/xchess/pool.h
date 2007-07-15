#ifndef XCHESS__POOL_H
#define XCHESS__POOL_H

#include <map>

template <typename Position>
struct PlayerPoolType {
  typedef std::map<typename Position::Piece::Type, int> type;
};

template <typename Position, typename PP>
class PoolReferenceBase {
protected:
  typedef typename Position::Piece Piece;
  typedef typename Piece::Color Color;
  typedef typename Piece::Type Type;
public:
  typedef std::map<Color, PP> Pool;
protected:
  PP* m_p_pool;
  const Color m_color;  
public:
  PoolReferenceBase(PP* p, Color color)
  : m_p_pool(p)
  , m_color(color) { }
  
  int size();
  Piece get(int idx);
};

template <typename Position>
class PoolReference : public PoolReferenceBase<Position, 
                          typename PlayerPoolType<Position>::type > {
public:
  typedef typename PlayerPoolType<Position>::type PlayerPool;
  typedef PoolReferenceBase<Position, PlayerPool> Base;
  typedef typename Base::Piece Piece;
  typedef typename Base::Type Type;
  typedef typename Base::Color Color;
  
  PoolReference(PlayerPool* p, Color color)
  : PoolReferenceBase<Position, PlayerPool>(p, color) { }
  
  int insert(int idx, const Piece& p);
  Piece take(int idx);
};

template <typename Position>
class PoolConstReference : public PoolReferenceBase<Position, 
                                  const typename PlayerPoolType<Position>::type> {
  typedef const typename PlayerPoolType<Position>::type PlayerPool;
  typedef typename Position::Piece::Color Color;
public:
  PoolConstReference(PlayerPool* p, Color color)
  : PoolReferenceBase<Position, const PlayerPool>(p, color) { }
};

template <typename Position, typename PP>
int PoolReferenceBase<Position, PP>::size() {
  if (!m_p_pool)
    return 0;

  int retv = 0;
  for (typename PP::const_iterator i = m_p_pool->begin(); i != m_p_pool->end(); ++i)
    retv += i->second;
  return retv;
}

template <typename Position, typename PP>
typename PoolReferenceBase<Position, PP>::Piece PoolReferenceBase<Position, PP>::get(int idx) {
  if (idx < 0)
    return Piece();

  int fill = 0;
  for (typename PP::const_iterator i = m_p_pool->begin(); i != m_p_pool->end(); ++i) {
    if (idx < fill + i->second)
      return Piece(m_color, i->first);
    fill += i->second;
  }
  return Piece();
}

template <typename Position>
int PoolReference<Position>::insert(int idx, const Piece& p) {
  if (this->m_color != p.color()) {
    ERROR("Inserting a piece in the wrong pool?");
    return -1;
  }

  int fill = 0;
  for (typename PlayerPool::iterator i = this->m_p_pool->begin(); 
        (i != this->m_p_pool->end()) && i->first < p.type(); 
        ++i) {
    fill += i->second;
  }
  
  int nump = ++(*this->m_p_pool)[p.type()];

  if (idx < fill)
    return fill;
  if (idx >= fill + nump)
    return fill + nump - 1;
  return idx;
}


template <typename Position>
typename PoolReference<Position>::Piece PoolReference<Position>::take(int idx) {
  if (idx < 0)
    return Piece();

  int fill = 0;
  for(typename PlayerPool::iterator i = this->m_p_pool->begin(); 
        i != this->m_p_pool->end(); 
        ++i) {
    if(idx < fill + i->second) {
      Type t = i->first;
      if(!--i->second)
        this->m_p_pool->erase(i);
      return Piece(this->m_color, t);
    }
    fill += i->second;
  }
  return Piece();
}

#endif // XCHESS__POOL_H

