#ifndef XCHESS__DROPANIMATOR_IMPL_H
#define XCHESS__DROPANIMATOR_IMPL_H

#include "dropanimator.h"
#include "animator.impl.h"

template <typename Variant>
DropAnimator<Variant>::DropAnimator(API cinterface)
: Base(cinterface) { }

template <typename Variant>
void DropAnimator<Variant>::updatePool(const Position& final) {
  for(int color = 0; color < 2; color++) {
//     typename Piece::Color c = static_cast<typename Piece::Color>(color);
//     const typename Position::PlayerPool& before = m_cinterface->position()->rawPool().find(c)->second;
//     const typename Position::PlayerPool& after = final.rawPool().find(c)->second;
//     typename Position::PlayerPool::const_iterator before_it = before.begin();
//     typename Position::PlayerPool::const_iterator after_it = after.begin();
    
    typename Position::Color c = static_cast<typename Piece::Color>(color);
    const typename Position::PlayerPool& before = const_cast<Position*>(
                                                    m_cinterface->position())->rawPool()[c];
    const typename Position::PlayerPool& after = const_cast<Position*>(
                                                                  &final)->rawPool()[c];
    typename Position::PlayerPool::const_iterator before_it = before.begin();
    typename Position::PlayerPool::const_iterator after_it = after.begin();

    int pos = 0;

    // oh, a nice bunch of write-only magic shit
    while(before_it != before.end() || after_it != after.end()) {
      if(after_it == after.end() || (before_it != before.end()
              && before_it->first < after_it->first )) {
        for(int i=0;i<before_it->second;i++)
          m_cinterface->takePoolSprite(color, pos);
        ++before_it;
      }
      else if (before_it == before.end() || (after_it != after.end()
              && after_it->first < before_it->first )) {
        for(int i=0;i<after_it->second;i++)
          m_cinterface->insertPoolPiece(color, pos, Piece(c, after_it->first) );
        pos += after_it->second;
        ++after_it;
      }
      else {
        Q_ASSERT(after_it->first == before_it->first);
        if(before_it->second < after_it->second)
        for(int i=0;i<after_it->second - before_it->second;i++)
          m_cinterface->insertPoolPiece(color, pos, Piece(c, after_it->first) );
        else if(before_it->second > after_it->second)
        for(int i=0;i<before_it->second - after_it->second;i++)
          m_cinterface->takePoolSprite(color, pos);
        pos += after_it->second;
        ++after_it;
        ++before_it;
      }
    }
    
  }
}

/*
template <typename Variant>
void DropAnimator<Variant>::updatePool(const Position& final) {
  final.dump();

  for(int color = 0; color < 2; color++) {
    typename Piece::Color c = static_cast<typename Piece::Color>(color);
    const typename Position::PlayerPool& before = m_cinterface->position()->rawPool().find(c)->second;
    const typename Position::PlayerPool& after = final.rawPool().find(c)->second;
    typename Position::PlayerPool::const_iterator before_it = before.begin();
    typename Position::PlayerPool::const_iterator after_it = after.begin();

    int pos = 0;

    // oh, a nice bunch of write-only magic shit
    while(before_it != before.end() || after_it != after.end()) {
      if(after_it == after.end() || (before_it != before.end()
              && before_it->first < after_it->first )) {
        for(int i=0;i<before_it->second;i++)
          m_cinterface->takePoolSprite(color, pos);
        ++before_it;
      }
      else if (before_it == before.end() || (after_it != after.end()
              && after_it->first < before_it->first )) {
        for(int i=0;i<after_it->second;i++)
          m_cinterface->insertPoolPiece(color, pos, Piece(c, after_it->first) );
        pos += after_it->second;
        ++after_it;
      }
      else {
        Q_ASSERT(after_it->first == before_it->first);
        if(before_it->second < after_it->second)
        for(int i=0;i<after_it->second - before_it->second;i++)
          m_cinterface->insertPoolPiece(color, pos, Piece(c, after_it->first) );
        else if(before_it->second > after_it->second)
        for(int i=0;i<before_it->second - after_it->second;i++)
          m_cinterface->takePoolSprite(color, pos);
        pos += after_it->second;
        ++after_it;
        ++before_it;
      }
    }
  }
}*/

#endif // XCHESS__DROPANIMATOR_IMPL_H
