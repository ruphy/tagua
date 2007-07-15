#ifndef XCHESS__DROPANIMATOR_H
#define XCHESS__DROPANIMATOR_H

#include "animator.h"

template <typename Variant>
class DropAnimator : public SimpleAnimator<Variant> {
  typedef SimpleAnimator<Variant> Base;
  
  using Base::m_cinterface;
protected:
  typedef typename Base::API API;
  typedef typename Base::Position Position;
  typedef typename Base::Piece Piece;
  typedef typename Base::Move Move;
  
  virtual void updatePool(const Position& final);
public:
  DropAnimator(API cinterface);
  
  virtual AnimationGroupPtr forward(const Position& final, const Move& move);
  virtual AnimationGroupPtr back(const Position& final, const Move& move);
};

#endif // XCHESS__DROPANIMATOR_H

