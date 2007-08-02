#ifndef HLVARIANT__POOL_H
#define HLVARIANT__POOL_H

#include <map>

namespace HLVariant {

template <typename _Piece>
class Pool {
public:
  typedef _Piece Piece;
private:
  typedef typename Piece::Color Color;
  typedef typename Piece::Type Type;
  typedef std::map<Type, int> Data;
  
  Color m_owner;
  Data m_data;
public:
  Pool(Color owner);
  virtual ~Pool();
  
  virtual bool operator==(const Pool<Piece>& other) const;
  virtual bool operator!=(const Pool<Piece>& other) const;
  
  virtual int count(Type type) const;
  virtual int add(Type type);
  virtual int remove(Type type);
  
  virtual bool empty() const;
  virtual int size() const;
  virtual int insert(int index, const Piece& piece);
  virtual Piece get(int index) const;
  virtual Piece take(int index);
  
  typedef Data RawData;
  const RawData& rawData() const { return m_data; }
};


// IMPLEMENTATION

template <typename Piece>
Pool<Piece>::Pool(Color owner)
: m_owner(owner) { }

template <typename Piece>
Pool<Piece>::~Pool() { }

template <typename Piece>
bool Pool<Piece>::operator==(const Pool<Piece>& other) const {
  return m_owner == other.m_owner && m_data == other.m_data;
}

template <typename Piece>
bool Pool<Piece>::operator!=(const Pool<Piece>& other) const {
  return !((*this) == other);
}

template <typename Piece>
int Pool<Piece>::count(Type type) const {
  typename Data::const_iterator it = m_data.find(type);
  if (it != m_data.end()) {
    return it->second;
  }
  else {
    return 0;
  }
}

template <typename Piece>
int Pool<Piece>::add(Type type) {
  return ++m_data[type];
}

template <typename Piece>
int Pool<Piece>::remove(Type type) {
  int n = --m_data[type];
  if (n <= 0) {
    m_data.erase(type);
    return 0;
  }
  
  return n;
}

template <typename Piece>
bool Pool<Piece>::empty() const {
  return m_data.empty();
}

template <typename Piece>
int Pool<Piece>::size() const {
  int count = 0;
  for (typename Data::const_iterator end = m_data.end(), it = m_data.begin(); it != end; ++it)
    count += it->second;
  return count;
}

template <typename Piece>
int Pool<Piece>::insert(int index, const Piece& piece) {
  if (m_owner != piece.color())
    return -1;

  int fill = 0;
  for (typename Data::iterator end = m_data.end(), i = m_data.begin();
       i != end && i->first < piece.type(); 
       ++i) {
    fill += i->second;
  }

  int nump = add(piece.type());

  if (index < fill)
    return fill;
  if (index >= fill + nump)
    return fill + nump - 1;
  return index;
}

template <typename Piece>
Piece Pool<Piece>::get(int index) const {
  if (index < 0)
    return Piece();

  int fill = 0;
  for (typename Data::const_iterator end = m_data.end(), i = m_data.begin(); i != end; ++i) {
    if (index < fill + i->second)
      return Piece(m_owner, i->first);
    fill += i->second;
  }
  
  return Piece();
}

template <typename Piece>
Piece Pool<Piece>::take(int index) {
  if (index < 0)
    return Piece();

  int fill = 0;
  for (typename Data::iterator end = m_data.end(), i = m_data.begin(); i != end; ++i) {
    if(index < fill + i->second) {
      Type type = i->first;
      remove(type);
      return Piece(m_owner, type);
    }
    
    fill += i->second;
  }
  
  return Piece();
}

}

#endif // HLVARIANT__POOL_H

