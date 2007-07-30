#ifndef HLVARIANT__TAGUA_WRAPPED_H
#define HLVARIANT__TAGUA_WRAPPED_H

#include "tagua.h"
#include "fwd.h"
#include "nopool.h"

#ifdef Q_CC_GNU
  #define __FUNC__ __PRETTY_FUNCTION__
#else
  #define __FUNC__ __FUNCTION__
#endif

#define MISMATCH(x,y) (std::cout << " --> Error in "<<__FUNC__<<", MISMATCH!" << std::endl \
                       << "     got type   " << prettyTypeName(typeid(x).name()) << std::endl \
                       << "     instead of " << prettyTypeName(typeid(y).name()) << std::endl \
                       << "     this is    " << prettyTypeName(typeid(*this).name()) << std::endl)

namespace HLVariant {

  template <typename Variant> class WrappedPosition;

  template <typename Variant>
  class WrappedPool { };
  
  template <typename Variant>
  class WrappedPiece : public AbstractPiece {
    typedef typename Variant::LegalityCheck::GameState::Board::Piece Piece;
  
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
  class WrappedMove : public AbstractMove {
    typedef typename Variant::LegalityCheck LegalityCheck;
    typedef typename LegalityCheck::Move Move;
    typedef typename LegalityCheck::GameState GameState;

    Move m_move;
  public:
    const Move& inner() const { return m_move; }
    Move& inner() { return m_move; }
  
    WrappedMove(const Move& move)
    : m_move(move) { }
  
    virtual QString SAN(const PositionPtr& _ref) const {
      WrappedPosition<Variant>* ref = dynamic_cast<WrappedPosition<Variant>*>(_ref.get());
  
      if (ref) {
//         MoveSerializer<Position> serializer(m_move, pos->inner());
        return ""; //BROKEN
      }
      else {
        MISMATCH(*_ref.get(), WrappedPosition<Variant>);
        return "$@%";
      }
    }
  
    virtual DecoratedMove toDecoratedMove(const PositionPtr& _pos) const {
      return DecoratedMove(); // BROKEN
    }
  
    virtual QString toString(const PositionPtr& _pos) const {
      return ""; // BROKEN
    }
  
    virtual NormalUserMove toUserMove() const {
      return NormalUserMove(); // BROKEN
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
    static PoolPtr apply(typename Variant::GameState& state, int player) {
      return PoolPtr(new WrappedPool<Variant>(state.pool(player)));
    }
  };
  
  template <typename Variant>
  struct ReturnPoolAux<Variant, NoPool> {
    static PoolPtr apply(typename Variant::GameState&, int) {
      return PoolPtr();
    }
  };
  
  template <typename Variant>
  struct ReturnPool {
    static PoolPtr apply(typename Variant::GameState& state, int player) {
      return ReturnPoolAux<Variant, typename Variant::GameState::Pool>(state, player);
    }
  };

  template <typename Variant>
  class WrappedPosition : public AbstractPosition {
    typedef typename Variant::LegalityCheck LegalityCheck;
    typedef typename LegalityCheck::GameState GameState;
    typedef typename GameState::Board Board;
    typedef typename Board::Piece Piece;
    typedef typename GameState::Move Move;
    
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
      // BROKEN
      return QStringList();
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
  
    virtual PoolPtr pool(int) {
      // BROKEN
      return PoolPtr();
    }
    
    virtual void copyPoolFrom(const PositionPtr&) {
      // BROKEN
    }
  
    virtual InteractionType movable(const TurnTest&, const Point&) const {
      return Moving; // BROKEN
    }
  
    virtual InteractionType droppable(const TurnTest&, int) const {
      return Moving; // BROKEN
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
  
    virtual MovePtr getMove(const AlgebraicNotation&) const {
      // BROKEN
      return MovePtr();
    }
  
    virtual MovePtr getMove(const QString&) const {
      // BROKEN
      return MovePtr();
    }
  
    virtual QString state() const {
      return ""; // TODO
    }
  
    virtual QString fen(int, int) const {
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

#endif // HLVARIANT__TAGUA_WRAPPED_H

