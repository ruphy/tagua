#ifndef HLVARIANT__POOL_H
#define HLVARIANT__POOL_H

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

}

#endif // HLVARIANT__POOL_H

