#ifndef HLVARIANT__TAGUA_WRAPPED_H
#define HLVARIANT__TAGUA_WRAPPED_H

#include "tagua.h"
#include "fwd.h"
#include "movefactory.h"
#include "nopool.h"
#include "variantdata.h"

#ifdef Q_CC_GNU
  #define __FUNC__ __PRETTY_FUNCTION__
#else
  #define __FUNC__ __FUNCTION__
#endif

#define MISMATCH(x,y) (std::cout << " --> Error in "<<__FUNC__<<", MISMATCH!" << std::endl \
                       << "     got type   " << prettyTypeName(typeid(x).name()) << std::endl \
                       << "     instead of " << prettyTypeName(typeid(y).name()) << std::endl \
                       << "     this is    " << prettyTypeName(typeid(*this).name()) << std::endl)
                       
#define DEFINE_VARIANT_FACTORY() \
  VariantFactory* createFactory() { \
    return new WrappedVariantFactory<Variant>; \
  }

namespace HLVariant {

  template <typename Variant> class WrappedPosition;
  
  template <typename Variant>
  class WrappedPiece : public AbstractPiece {
    typedef typename VariantData<Variant>::Piece Piece;
  
    Piece m_piece;
  public:
    const Piece& inner() const { return m_piece; }
  
    WrappedPiece(const Piece& piece)
    : m_piece(piece) { }
  
    virtual bool equals(const PiecePtr& _other) const {
      if (!_other) return false;
      WrappedPiece<Variant>* other = dynamic_cast<WrappedPiece<Variant>*>(_other.get());
  
      if (other)
        return m_piece == other->inner();
      else {
        MISMATCH(*_other.get(),WrappedPiece<Variant>);
        return false;
      }
    }
  
    virtual QString name() const {
      return m_piece.name();
    }
  
    virtual PiecePtr clone() const {
      return PiecePtr(new WrappedPiece<Variant>(m_piece));
    }
  };
  
  template <typename Variant>
  class WrappedPool : public AbstractPool {
    typedef typename VariantData<Variant>::Pool Pool;
    typedef typename VariantData<Variant>::Piece Piece;
    
    Pool& m_pool;
  public:
    WrappedPool(Pool& pool)
    : m_pool(pool) { }
  
    WrappedPool(const Pool& pool)
    : m_pool(const_cast<Pool&>(pool)) {
      // FIXME: const_cast is not necessary, here, but to avoid it
      // we would need to change the API, adding for example an
      // AbstractConstPool with only const functions and changing the
      // wrapper accordingly.
    }
    
    virtual int size() {
      return m_pool.size();
    }
  
    virtual int insert(int pref_index, const PiecePtr& _piece) {
      if (!_piece) {
        return m_pool.insert(pref_index, Piece());
      }
      else {
        WrappedPiece<Variant>* piece = dynamic_cast<WrappedPiece<Variant>*>(_piece.get());
  
        if (piece)
          return m_pool.insert(pref_index, Piece(piece->inner()) );
        else {
          MISMATCH(*_piece.get(), WrappedPiece<Variant>);
          return -1;
        }
      }
    }
  
    virtual PiecePtr get(int index) {
      Piece piece = m_pool.get(index);
      if (piece != Piece())
        return PiecePtr(new WrappedPiece<Variant>(piece));
      else
        return PiecePtr();
    }
  
    virtual PiecePtr take(int index) {
      Piece piece = m_pool.take(index);
      if (piece != Piece())
        return PiecePtr(new WrappedPiece<Variant>(piece));
      else
        return PiecePtr();
    }
  };
  
  template <typename Variant>
  class WrappedMove : public AbstractMove {
    typedef typename VariantData<Variant>::LegalityCheck LegalityCheck;
    typedef typename VariantData<Variant>::Serializer Serializer;
    typedef typename VariantData<Variant>::Move Move;
    typedef typename VariantData<Variant>::GameState GameState;

    Move m_move;
  public:
    const Move& inner() const { return m_move; }
    Move& inner() { return m_move; }
  
    WrappedMove(const Move& move)
    : m_move(move) { }
  
    virtual QString toString(const QString& rep, const PositionPtr& _ref) const {
      WrappedPosition<Variant>* ref = dynamic_cast<WrappedPosition<Variant>*>(_ref.get());
  
      if (ref) {
        Serializer serializer(rep);
        return serializer.serialize(m_move, ref->inner());
      }
      else {
        MISMATCH(*_ref.get(), WrappedPosition<Variant>);
        return "$@%";
      }      
    }
  
    virtual NormalUserMove toUserMove() const {
      typename VariantData<Variant>::MoveFactory factory;
      return factory.toNormal(m_move);
    }
  
    virtual bool equals(const MovePtr& _other) const {
      WrappedMove<Variant>* other = dynamic_cast<WrappedMove<Variant>*>(_other.get());
  
      if (other)
        return m_move == other->inner();
      else {
        MISMATCH(*_other.get(), WrappedMove<Variant>);
        return false;
      }
    }
  };

  /**
    * Metafunction that returns a null pointer when
    * its template argument is NoPool.
    */
  template <typename Variant, typename Pool>
  struct ReturnPoolAux {
    static PoolPtr apply(typename VariantData<Variant>::GameState& state, int player) {
      return PoolPtr(new WrappedPool<Variant>(state.pools().pool(
        static_cast<typename VariantData<Variant>::Piece::Color>(player))));
    }
  };
  
  template <typename Variant>
  struct ReturnPoolAux<Variant, NoPool> {
    static PoolPtr apply(typename VariantData<Variant>::GameState&, int) {
      return PoolPtr();
    }
  };
  
  template <typename Variant>
  struct ReturnPool {
    static PoolPtr apply(typename VariantData<Variant>::GameState& state, int player) {
      return ReturnPoolAux<Variant, typename VariantData<Variant>::Pool>::apply(state, player);
    }
  };

  template <typename Variant>
  class WrappedPosition : public AbstractPosition {
    typedef typename VariantData<Variant>::LegalityCheck LegalityCheck;
    typedef typename VariantData<Variant>::GameState GameState;
    typedef typename VariantData<Variant>::Board Board;
    typedef typename VariantData<Variant>::Piece Piece;
    typedef typename VariantData<Variant>::Move Move;
    typedef typename VariantData<Variant>::Serializer Serializer;
    
    GameState m_state;
  public:
    const GameState& inner() const { return m_state; }
    GameState& inner() { return m_state; }
    
    WrappedPosition(const GameState& state)
    : m_state(state) { }
    
    virtual Point size() const {
      return m_state.board().size();
    }

    virtual QStringList borderCoords() const {
      return m_state.board().borderCoords();
    }
  
    virtual void setup() {
      m_state.setup();
    }
  
    virtual PiecePtr get(const Point& p) const {
      Piece piece = m_state.board().get(p);
      if (piece != Piece())
        return PiecePtr(new WrappedPiece<Variant>(piece));
      else
        return PiecePtr();
    }
  
    virtual void set(const Point& p, const PiecePtr& _piece) {
      if (!_piece) {
        m_state.board().set(p, Piece());
      }
      else {
        WrappedPiece<Variant>* piece = dynamic_cast<WrappedPiece<Variant>*>(_piece.get());
  
        if (piece)
          m_state.board().set(p, piece->inner());
        else
          MISMATCH(*_piece.get(), WrappedPiece<Variant>);
      }
    }
  
    virtual PoolPtr pool(int player) {
      return ReturnPool<Variant>::apply(m_state, player);
    }
    
    virtual void copyPoolFrom(const PositionPtr&) {
      // BROKEN
    }
  
    virtual InteractionType movable(const TurnTest& test, const Point& p) const {
      LegalityCheck check(m_state);
      return check.movable(test, p);
    }
  
    virtual InteractionType droppable(const TurnTest& test, int index) const {
      LegalityCheck check(m_state);
      return check.droppable(test, index);
    }
  
    virtual int turn() const {
      return static_cast<int>(m_state.turn());
    }
  
    virtual void setTurn(int turn) {
      m_state.setTurn(static_cast<typename Piece::Color>(turn));
    }
  
    virtual int previousTurn() const {
      return static_cast<int>(m_state.previousTurn());
    }
  
    virtual void switchTurn() {
      m_state.switchTurn();
    }
  
    virtual bool testMove(const MovePtr& _move) const {
      if (!_move)
        return false;
        
      WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());
  
      if (move) {
        LegalityCheck check(m_state);
        return check.legal(move->inner());
      }
      else {
        MISMATCH(*_move.get(), WrappedMove<Variant>);
        return false;
      }
    }
  
    virtual void move(const MovePtr& _move) {
      if (!_move)
        return;
        
      WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());
  
      if (move)
        m_state.move(move->inner());
      else
        MISMATCH(*_move.get(), WrappedMove<Variant>);
    }
  
    virtual PositionPtr clone() const {
      return PositionPtr(new WrappedPosition<Variant>(m_state));
    }
  
    virtual void copyFrom(const PositionPtr& _p) {
      // TODO: check if this is used somewhere
      WrappedPosition<Variant>* p = dynamic_cast<WrappedPosition<Variant>*>(_p.get());
  
      if (p)
        m_state = p->inner();
      else
        MISMATCH(*_p.get(), WrappedPosition);
    }
  
    virtual bool equals(const PositionPtr& _other) const {
      WrappedPosition<Variant>* other = dynamic_cast<WrappedPosition<Variant>*>(_other.get());
  
      if (other)
        return m_state == other->inner();
      else {
        MISMATCH(*_other.get(), WrappedPosition<Variant>);
        return false;
      }
    }
  
    virtual MovePtr getMove(const QString& san) const {
      Serializer serializer("compact");
      Move res = serializer.deserialize(san, m_state);
      if (res.valid()) {
        return MovePtr(new WrappedMove<Variant>(res));
      }
      else
        return MovePtr();
    }
  
    virtual QString state() const {
      return ""; // BROKEN
    }

    virtual PiecePtr moveHint(const MovePtr&) const {
      return PiecePtr(); // BROKEN
    }
  
    virtual QString variant() const {
      return Variant::m_name;
    }
  
    virtual void dump() const {
      // BROKEN
    }
    
  };
}

#include "graphicalapi_unwrapped.h"
#include "icsapi_wrapped.h"

namespace HLVariant {
  
  template <typename Variant>
  class WrappedAnimator : public AbstractAnimator {
    typedef typename VariantData<Variant>::GameState GameState;
    typedef typename VariantData<Variant>::Animator Animator;
    typedef typename VariantData<Variant>::Move Move;
  
    Animator m_animator;
  public:
    const Animator& inner() const { return m_animator; }
  
    WrappedAnimator(const Animator& animator)
    : m_animator(animator) { }
  
    virtual AnimationPtr warp(const PositionPtr& _pos) {
      WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
      if (pos)
        return m_animator.warp(pos->inner());
      else {
        MISMATCH(*_pos.get(), WrappedPosition<Variant>);
        return AnimationPtr();
      }
    }
  
    virtual AnimationPtr forward(const PositionPtr& _pos, const MovePtr& _move) {
      WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
      WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());
  
      if (move && pos)
        return m_animator.forward(pos->inner(), move->inner());
      else {
        if (!move)
          MISMATCH(*_move.get(), WrappedMove<Variant>);
        if (!pos)
          MISMATCH(*_pos.get(), WrappedPosition<Variant>);
        return AnimationPtr();
      }
    }
  
    virtual AnimationPtr back(const PositionPtr& _pos, const MovePtr& _move) {
      WrappedPosition<Variant>* pos = dynamic_cast<WrappedPosition<Variant>*>(_pos.get());
      WrappedMove<Variant>* move = dynamic_cast<WrappedMove<Variant>*>(_move.get());
  
      if (move && pos)
        return m_animator.back(pos->inner(), move->inner());
      else {
        if (!move)
          MISMATCH(*_move.get(), WrappedMove<Variant>);
        if (!pos)
          MISMATCH(*_pos.get(), WrappedPosition<Variant>);
        return AnimationPtr();
      }
    }
  };

  
  
  template <typename Variant>
  class WrappedVariantInfo : public VariantInfo {
    typedef typename VariantData<Variant>::Animator Animator;
    typedef typename VariantData<Variant>::GameState GameState;
    typedef typename VariantData<Variant>::Piece Piece;
    typedef typename VariantData<Variant>::Move Move;
//     typedef typename VariantData<Variant>::Pool Pool;

    Variant m_variant;
  public:
    virtual PositionPtr createPosition() {
      return PositionPtr(
        new WrappedPosition<Variant>(GameState()));
    }
  
    virtual PositionPtr createCustomPosition(const OptList&) {
      return PositionPtr(
        new WrappedPosition<Variant>(GameState())); // BROKEN
    }
    
    virtual void forallPieces(class PieceFunction&) {
      // BROKEN
    }
    
    virtual int moveListLayout() const {
      return Variant::moveListLayout();
    }
    
    virtual AnimatorPtr createAnimator(GraphicalAPI* graphical_api) {
      return AnimatorPtr(new WrappedAnimator<Variant>(
        Animator(
          typename UnwrappedGraphicalAPIPtr<Variant>::type(
            new UnwrappedGraphicalAPI<Variant>(graphical_api)))));
    }
    
    virtual MovePtr createNormalMove(const NormalUserMove& move) {
      typename VariantData<Variant>::MoveFactory factory;
      Move m = factory.createNormalMove(move);
      return MovePtr(new WrappedMove<Variant>(m));
    }
    
    virtual MovePtr createDropMove(const DropUserMove& move) {
      typename VariantData<Variant>::MoveFactory factory;
      Move m = factory.createDropMove(move);
      return MovePtr(new WrappedMove<Variant>(m));
    }
  
    virtual MovePtr getVerboseMove(int, const VerboseNotation&) const {
      return MovePtr();
    }
    
    virtual bool simpleMoves() {
      return Variant::m_simple_moves;
    }
    
    virtual QString name() const {
      return Variant::m_name;
    }
    
    virtual QString themeProxy() const {
      return Variant::m_theme_proxy;
    }
    
    virtual OptList positionOptions() const {
      return m_variant.positionOptions();
    }
    
    virtual ICSAPIPtr icsAPI() const {
      return ICSAPIPtr(ReturnICSAPI<Variant>::apply());
    }
    
    virtual ActionCollection* actions() {
      return m_variant.actions();
    }
    
    virtual void setupMove(NormalUserMove& m) const {
      m_variant.setupMove(m);
    }
  };
  
  template <typename Variant>
  class WrappedVariantFactory : public VariantFactory {
  public:
    virtual VariantInfo* createVariant() const {
      return new WrappedVariantInfo<Variant>;
    }
    
    virtual QString name() const {
      return Variant::m_name;
    }
    
    virtual QString themeProxy() const {
      return Variant::m_theme_proxy;
    }
    
    virtual bool hidden() const {
      return Variant::m_hidden;
    }
  };
  
}

#endif // HLVARIANT__TAGUA_WRAPPED_H

