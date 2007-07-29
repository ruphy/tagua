#ifndef HLVARIANT__BOARD_H
#define HLVARIANT__BOARD_H

#include <vector>

#include "point.h"
#include "pathinfo.h"

namespace HLVariant {

template <typename _Piece>
class Board {
public:
  typedef _Piece Piece;
private:
  Point m_size;
  std::vector<Piece> m_data;
public:
  /**
    * Create a new Board with the given size.
    */
  Board(const Point& size);
  
  /**
    * Create a Board copying a given one.
    */
  Board(const Board<Piece>& other);
  
  /**
    * Compare two boards for equality.
    */
  bool operator==(const Board& other) const;
  bool operator!=(const Board& other) const;
  
  /**
    * \return Board size.
    */
  Point size() const;
  
  /**
    * Retrieve a piece from the board.
    * \param p Coordinates of the piece.
    * \return The piece at that square, or an invalid piece, if the square is out of the board.
    */
  Piece get(const Point& p) const;
  
  /**
    * Add a piece to the board.
    * \param p Coordinates of the piece.
    * \param piece The piece to add.
    * \note This function does nothing if @a p is out of the board.
    */
  void set(const Point& p, const Piece& piece);
  
  /**
    * \return Whether @a p is a valid board square.
    */
  bool valid(const Point& p) const;
  
  /**
    * \return Information on the path joining @a from and @a to.
    */
  PathInfo path(const Point& from, const Point& to) const;
};

// IMPLEMENTATION

template <typename Piece>
Board<Piece>::Board(const Point& size)
: m_size(size) {
  m_data.resize(m_size.x * m_size.y);
}

template <typename Piece>
Board<Piece>::Board(const Board<Piece>& other)
: m_size(other.m_size)
, m_data(other.m_data) { }

template <typename Piece>
bool Board<Piece>::operator==(const Board<Piece>& other) const {
  if (m_size != other.m_size)
    return false;
    
  const unsigned int total = m_data.size();
  for (unsigned int i = 0; i < total; i++) {
    if (m_data[i] != other.m_data[i])
      return false;
  }
  
  return true;
}

template <typename Piece>
bool Board<Piece>::operator!=(const Board<Piece>& other) const {
  return !((*this) == other);
}

template <typename Piece>
Point Board<Piece>::size() const { return m_size; }

template <typename Piece>
Piece Board<Piece>::get(const Point& p) const {
  if (valid(p)) {
    return m_data[p.x + p.y * m_size.x];
  }
  else {
    return Piece();
  }
}

template <typename Piece>
void Board<Piece>::set(const Point& p, const Piece& piece) {
  if (valid(p)) {
    m_data[p.x + p.y * m_size.x] = piece;
  }
}

template <typename Piece>
bool Board<Piece>::valid(const Point& p) const {
  return p.x < m_size.x &&
         p.x >= 0       &&
         p.y < m_size.y &&
         p.y >= 0;
}

template <typename Piece>
PathInfo Board<Piece>::path(const Point& from, const Point& to) const {
  if (!valid(from) || !valid(to))
    return PathInfo(PathInfo::Undefined, 0);
    
  Point delta = to - from;
  PathInfo::Direction direction;

  if (delta.x == 0)
    direction = PathInfo::Vertical;
  else if (delta.y == 0)
    direction = PathInfo::Horizontal;
  else if (delta.x == delta.y)
    direction = PathInfo::Diagonal1;
  else if (delta.x == -delta.y)
    direction = PathInfo::Diagonal2;
  else
    direction = PathInfo::Undefined;

  int num_obs = 0;
  if (direction != PathInfo::Undefined) {
    Point step = delta.normalizeInfinity();
    Point position = from;
    while ((position += step) != to) {
      if (get(position) == Piece())
        continue;
      num_obs++;
    }
  }

  return PathInfo(direction, num_obs);
}

}

#endif // HLVARIANT__BOARD_H
