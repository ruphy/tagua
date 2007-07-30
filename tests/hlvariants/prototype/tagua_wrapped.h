#ifndef HLVARIANT__TAGUA_WRAPPED_H
#define HLVARIANT__TAGUA_WRAPPED_H

#include "tagua.h"
#include "fwd.h"

namespace HLVariant {

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
    
    virtual void copyPoolFrom(AbstractPosition::Ptr) {
      // BROKEN
    }
  
    virtual InteractionType movable(const TurnTest& test, const Point& p) const {
      return Moving; // BROKEN
    }
  
    virtual InteractionType droppable(const TurnTest& test, int p) const {
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
  
    virtual AbstractMove::Ptr getMove(const QString&) const {
      // BROKEN
      return MovePtr();
    }
  
    virtual QString state() const {
      return ""; // TODO
    }
  
    virtual QString fen(int, int) const {
      return ""; // BROKEN
    }
  
    virtual AbstractPiece::Ptr moveHint(const MovePtr&) const {
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

