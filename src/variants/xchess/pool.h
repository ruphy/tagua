#ifndef XCHESS__POOL_H
#define XCHESS__POOL_H

template <typename Position>
class PoolReference {
  typedef typename Position::Piece Piece;
  typedef typename Piece::Color Color;
  typedef typename Piece::Type Type;
public:
  typedef std::map<Type, int> PlayerPool;
  typedef std::map<Color, PlayerPool> Pool;
private:  
  PlayerPool* m_p_pool;
  Color m_color;
public:
  PoolReference(PlayerPool* p, Color color)
    : m_p_pool(p)
    , m_color(color) {
  }
  
  int size();
  int insert(int idx, const Piece& p);
  Piece get(int idx);
  Piece take(int idx);
};


template <typename Position>
int PoolReference<Position>::size() {
  if (!m_p_pool)
    return 0;

  int retv = 0;
  for (typename PlayerPool::iterator i = m_p_pool->begin(); i != m_p_pool->end(); ++i)
    retv += i->second;
  return retv;
}

template <typename Position>
int PoolReference<Position>::insert(int idx, const Piece& p) {
  if (m_color != p.color()) {
    ERROR("Inserting a piece in the wrong pool?");
    return -1;
  }

  int fill = 0;
  for (typename PlayerPool::iterator i = m_p_pool->begin(); 
        (i != m_p_pool->end()) && i->first < p.type(); 
        ++i) {
    fill += i->second;
  }
  
  int nump = ++(*m_p_pool)[p.type()];

  if (idx < fill)
    return fill;
  if (idx >= fill + nump)
    return fill + nump - 1;
  return idx;
}

template <typename Position>
typename PoolReference<Position>::Piece PoolReference<Position>::get(int idx) {
  if (idx < 0)
    return Piece();

  int fill = 0;
  for (typename PlayerPool::iterator i = m_p_pool->begin(); i != m_p_pool->end(); ++i) {
    if (idx < fill + i->second)
      return Piece(m_color, i->first);
    fill += i->second;
  }
  return Piece();
}

template <typename Position>
typename PoolReference<Position>::Piece PoolReference<Position>::take(int idx) {
  if (idx < 0)
    return Piece();

  int fill = 0;
  for(typename PlayerPool::iterator i = m_p_pool->begin(); i != m_p_pool->end(); ++i) {
    if(idx < fill + i->second) {
      Type t = i->first;
      if(!--i->second)
        m_p_pool->erase(i);
      return Piece(m_color, t);
    }
    fill += i->second;
  }
  return Piece();
}

#endif // XCHESS__POOL_H

