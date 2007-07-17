#ifndef XCHESS__DROPANIMATOR_IMPL_H
#define XCHESS__DROPANIMATOR_IMPL_H

#include "dropanimator.h"
#include "animator.impl.h"

#include <KDebug>

template <typename Base>
DropAnimatorMixin<Base>::DropAnimatorMixin(API cinterface)
: Base(cinterface) { }


template <typename Base>
void DropAnimatorMixin<Base>::updatePool(const Position& final) {
  for(int color = 0; color < 2; color++) {
    typename Piece::Color c = static_cast<typename Piece::Color>(color);
    const typename Position::PlayerPool& before = m_cinterface->position()->rawPool(c);
    const typename Position::PlayerPool& after = final.rawPool(c);
    typename Position::PlayerPool::const_iterator before_it = before.begin();
    typename Position::PlayerPool::const_iterator after_it = after.begin();

    int pos = 0;

    // oh, a nice bunch of write-only magic shit
    while(before_it != before.end() || after_it != after.end()) {
      bool skip_after = (after_it == after.end() || (before_it != before.end()
                                && before_it->first < after_it->first ));
      bool skip_before = (before_it == before.end() || (after_it != after.end()
                                && after_it->first < before_it->first ));
      int na = skip_after ? 0 : after_it->second;
      int nb = skip_before ? 0 : before_it->second;

      if(nb < na) {
        for(int i = nb; i < na; i++)
          m_cinterface->insertPoolPiece(color, pos + (i - nb), Piece(c, after_it->first) );
      }
      else if(na < nb) {
        for(int i = na; i < nb; i++)
          m_cinterface->removePoolSprite(color, pos);
      }

      if(!skip_before)
        ++before_it;
      if(!skip_after) {
        pos += after_it->second;
        ++after_it;
      }
    }
  }
}

template <typename Base>
AnimationGroupPtr DropAnimatorMixin<Base>::warp(const Position& final) {
  updatePool(final);
  return Base::warp(final);
}

template <typename Base>
AnimationGroupPtr DropAnimatorMixin<Base>::forward(const Position& final, const Move& move) {
  AnimationFactory res(m_cinterface->inner());

  if(move.drop()) {
    std::pair<int, int> dropped = m_cinterface->droppedPoolPiece();
    if(dropped.first != -1 && dropped.second != -1
        && m_cinterface->position()->pool(dropped.first).get(dropped.second) == move.drop()) {
      NamedSprite drop = m_cinterface->takePoolSprite(dropped.first, dropped.second);
      m_cinterface->setSprite(move.to, drop);
      res.addPreAnimation(Animate::move(drop, move.to));
      return res;
    }
    else {
      NamedSprite drop = m_cinterface->setPiece(move.to, move.drop(), false);
      res.addPreAnimation(Animate::appear(drop));
    }
  }
  else {
    res.setGroup(Base::forward(final, move));
  }

  updatePool(final);
  return res;
}

template <typename Base>
AnimationGroupPtr DropAnimatorMixin<Base>::back(const Position& final, const Move& move) {
  AnimationFactory res(m_cinterface->inner());

  if(move.drop()) {
    NamedSprite drop = m_cinterface->takeSprite(move.to);
    res.addPostAnimation(Animate::disappear(drop));

  }
  else {
    res.setGroup(Base::back(final, move));
  }

  updatePool(final);
  return res;
}

#endif // XCHESS__DROPANIMATOR_IMPL_H
