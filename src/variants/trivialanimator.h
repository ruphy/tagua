#ifndef TRIVIALANIMATOR_H
#define TRIVIALANIMATOR_H

template <typename Variant>
class TrivialAnimator : public SimpleAnimator<Variant> {
protected:
  typedef SimpleAnimator<Variant> Base;
  typedef typename Base::AnimationPtr AnimationPtr;
  typedef typename Base::GPosition GPosition;
  typedef typename Base::GElement GElement;
  typedef typename Base::Position Position;
  typedef typename Base::Piece Piece;
  typedef typename Base::Move Move;
public:
  TrivialAnimator(PointConverter* converter, const boost::shared_ptr<GPosition>& position)
  : Base(converter, position) { }
  virtual ~TrivialAnimator() { }
  virtual AnimationPtr forward(const Position& pos, const Move&) { return warp(pos); }
  virtual AnimationPtr back(const Position& pos, const Move&) { return warp(pos); }
};

#endif // TRIVIALANIMATOR_H
