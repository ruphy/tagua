#ifndef TRIVIALANIMATOR_H
#define TRIVIALANIMATOR_H

template <typename Variant>
class TrivialAnimator {
  GenericAnimator(PointConverter* converter, const boost::shared_ptr<GPosition>& position);
  virtual ~GenericAnimator() { }
  virtual AnimationPtr warp(const Position&);
  virtual AnimationPtr forward(const Position&, const Move& move);
  virtual AnimationPtr back(const Position&, const Move& move);
};

#endif // TRIVIALANIMATOR_H
