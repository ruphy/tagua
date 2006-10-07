/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "editposition.h"
#include "graphicalinfo.h"
#include "entities/userentity.h"
#include "piecefunction.h"

using namespace boost;

//BEGIN UndoHistory

UndoHistory::UndoHistory() {
  m_index = 0;
}

void UndoHistory::add(const UndoAction& action) {
  m_history.erase(m_history.begin() + m_index,
                  m_history.end());
  m_history.push_back(action);
  m_index++;
}

const UndoAction* UndoHistory::undo() {
  if (m_index <= 0) return 0;

  m_index--;
  return &m_history[m_index];
}

const UndoAction* UndoHistory::redo() {
  if (m_index >= m_history.size()) return 0;

  UndoAction* res = &m_history[m_index];
  m_index++;
  return res;
}

//END UndoHistory

class EditAction : public AbstractMove {
  typedef variant<NormalUserMove, DropUserMove> UserMove;
  UserMove m_move;

  class ConvertToNormalUserMove : public static_visitor<NormalUserMove> {
  public:
    NormalUserMove operator()(const NormalUserMove& m) const {
      return m;
    }

    NormalUserMove operator()(const DropUserMove& m) const {
      return NormalUserMove(Point::invalid(), m.m_to);
    }
  };
public:
  EditAction(const UserMove& move)
  : m_move(move) { }
  virtual QString SAN(boost::shared_ptr<AbstractPosition>) const { return QString(); }
  virtual QString toString(boost::shared_ptr<AbstractPosition>) const { return QString(); }
  virtual NormalUserMove toUserMove() const {
    return apply_visitor(ConvertToNormalUserMove(), m_move);
  }
  virtual AbstractPiece::Ptr pieceHint() const {
    if (const DropUserMove* move = get<DropUserMove>(&m_move))
      return move->m_piece;
    else
      return AbstractPiece::Ptr();
  }
  virtual bool equals(AbstractMove::Ptr /*other*/) const {
//     EditAction* move = dynamic_cast<EditAction*>(other.get());
//     return move && (m_move == move->m_move);
    // FIXME: the above does not compile
    return false;
  }

  UserMove& move() { return m_move; }
};

class EditPositionEntity : public UserEntity {
  struct EditPositionEntityInternal {
    shared_ptr<GraphicalInfo> m_graphical;
    VariantInfo* m_variant;
    EditPositionEntityInternal(const shared_ptr<GraphicalInfo>& graphical,
                               VariantInfo* variant)
    : m_graphical(graphical)
    , m_variant(variant) { }
  };
  EditPositionEntityInternal m_data;
  UndoHistory& m_undo_history;

  class ExecuteMoveVisitor : public static_visitor<UndoAction> {
    EditPositionEntityInternal& m_data;
  public:
    ExecuteMoveVisitor(EditPositionEntityInternal& data)
    : m_data(data) { }

    UndoAction operator()(const NormalUserMove& move) {
      if (move.to.valid()) {
        Element removedElement = m_data.m_graphical->getElement(move.to);
        m_data.m_graphical->setElement(
          move.to,
          m_data.m_graphical->getElement(move.from));
        m_data.m_graphical->adjustSprite(move.to);
        m_data.m_graphical->removeElement(move.from);
        return MoveAction(move.from, move.to, removedElement.piece());
      }
      else {
        Element removedElement = m_data.m_graphical->getElement(move.from);
        m_data.m_graphical->removeElement(move.from);
        return RemoveAction(move.from, removedElement.piece());
      }
    }

    UndoAction operator()(const DropUserMove& move) {
      AbstractPiece::Ptr dropped = move.m_piece;
      m_data.m_graphical->setPiece(
        move.m_to,
        dropped,
        true);
      m_data.m_graphical->adjustSprite(move.m_to);
      return DropAction(move.m_to, dropped);
    }
  };

  class UndoActionVisitor : public static_visitor<void> {
    EditPositionEntityInternal& m_data;
  public:
    UndoActionVisitor(EditPositionEntityInternal& data)
    : m_data(data) { }

    void operator()(const DropAction& action) {
      m_data.m_graphical->removeElement(action.m_to);
    }

    void operator()(const RemoveAction& action) {
      if (action.m_piece)
        m_data.m_graphical->setPiece(action.m_to, action.m_piece);
    }

    void operator()(const MoveAction& action) {
      m_data.m_graphical->setElement(
        action.m_from,
        m_data.m_graphical->getElement(action.m_to));
      if (action.m_piece)
        m_data.m_graphical->setPiece(action.m_to, action.m_piece);
      else
        m_data.m_graphical->removeElement(action.m_to);
      m_data.m_graphical->adjustSprite(action.m_from);
    }

    void operator()(const WarpAction& action) {
      std::cout << "undo warp from:" << std::endl;
      std::cout << action.m_from->fen(0, 0) << std::endl;
      std::cout << "to" << std::endl;
      std::cout << action.m_to->fen(0, 0) << std::endl;
      m_data.m_graphical->warp(AbstractMove::Ptr(), action.m_from);
    }
  };

  class RedoActionVisitor : public static_visitor<void> {
    EditPositionEntityInternal& m_data;
  public:
    RedoActionVisitor(EditPositionEntityInternal& data)
    : m_data(data) { }

    void operator()(const DropAction& action) {
      if (action.m_piece)
        m_data.m_graphical->setPiece(action.m_to, action.m_piece);
    }

    void operator()(const RemoveAction& action) {
      m_data.m_graphical->removeElement(action.m_to);
    }

    void operator()(const MoveAction& action) {
      m_data.m_graphical->setElement(
        action.m_to,
        m_data.m_graphical->getElement(action.m_from));
      m_data.m_graphical->removeElement(action.m_from);
      m_data.m_graphical->adjustSprite(action.m_to);
    }

    void operator()(const WarpAction& action) {
      m_data.m_graphical->warp(AbstractMove::Ptr(), action.m_to);
    }
  };
public:
  EditPositionEntity(VariantInfo* variant,
                     const shared_ptr<GraphicalInfo>& graphical,
                     UndoHistory& undo_history)
  : UserEntity(0)
  , m_data(graphical, variant)
  , m_undo_history(undo_history) { }

  virtual QString save() const { return ""; }
  virtual void loadPGN(const PGN&) { }

  virtual AbstractMove::Ptr testMove(const NormalUserMove& m) const {
    return AbstractMove::Ptr(new EditAction(m));
  }

  virtual AbstractMove::Ptr testMove(const DropUserMove& m) const {
    return AbstractMove::Ptr(new EditAction(m));
  }

  virtual bool testPremove(const NormalUserMove&) const { return false; }
  virtual bool testPremove(const DropUserMove&) const { return false; }

  virtual void executeMove(AbstractMove::Ptr move) {
    EditAction* action = dynamic_cast<EditAction*>(move.get());
    if (action) {
      ExecuteMoveVisitor visitor(m_data);
      UndoAction undo_action = apply_visitor(visitor, action->move());
      m_undo_history.add(undo_action);
    }
  }

  virtual void handleRightClick(const Point& p) const {
    Element removedElement = m_data.m_graphical->getElement(p);
    UndoAction action = RemoveAction(p, removedElement.piece());
    m_data.m_graphical->removeElement(p);
    m_undo_history.add(action);
  }

  virtual void addPremove(const NormalUserMove&) { }
  virtual void addPremove(const DropUserMove&) { }
  virtual void cancelPremove() { }

  virtual Action validTurn(const Point&) const { return Moving; }
  virtual Action validTurn(int) const { return Moving; }
  virtual bool movable(const Point&) const { return true; }
  virtual bool jump(const Index&) { return false; }
  virtual bool goTo(const Index&) { return false; }
  virtual bool gotoFirst() { return false; }
  virtual bool gotoLast() { return false; }
  virtual bool forward() { return false; }
  virtual bool back() { return false; }

  virtual bool undo() {
    if (const UndoAction* action = m_undo_history.undo()) {
      UndoActionVisitor visitor(m_data);
      apply_visitor(visitor, *action);
      return true;
    }
    else return false;
  }

  virtual bool redo() {
    if (const UndoAction* action = m_undo_history.redo()) {
      RedoActionVisitor visitor(m_data);
      apply_visitor(visitor, *action);
      return true;
    }
    else return false;
  }

  virtual bool truncate() { return false; }
  virtual bool promoteVariation() { return false; }

  virtual void notifyClockUpdate(int, int) { }
  virtual void notifyMove(AbstractMove::Ptr, AbstractPosition::Ptr) { }
  virtual void notifyBack() { }
  virtual void notifyForward() { }
  virtual void notifyGotoFirst() { }
  virtual void notifyGotoLast() { }
};

class FillPool : public PieceFunction {
  shared_ptr<GraphicalInfo> m_graphical;
  VariantInfo* m_variant;
public:
  FillPool(VariantInfo* variant, const shared_ptr<GraphicalInfo>& graphical)
  : m_graphical(graphical)
  , m_variant(variant) { }

  virtual void operator()(int color, int type) {
    m_graphical->addToPool(
      m_variant->createPiece(color, type),
      1
    );
  }
};

EditPositionController::EditPositionController(ChessTable* view, VariantInfo* variant)
: Controller(view)
, m_variant(variant) {
  m_graphical = shared_ptr<GraphicalInfo>(
    new GraphicalInfo(m_view, AbstractPosition::Ptr(), m_variant));
  m_entity = shared_ptr<EditPositionEntity>(
    new EditPositionEntity(m_variant, m_graphical, m_undo_history));
  m_graphical->setup(m_entity);

  FillPool fill_pool(m_variant, m_graphical);
  m_variant->forallPieces(fill_pool);
}

shared_ptr<UserEntity> EditPositionController::entity() const {
  return m_entity;
}

bool EditPositionController::clearBoard() {
  AbstractPosition::Ptr position = m_variant->createPosition();
  m_undo_history.add(WarpAction(m_graphical->position()->clone(), position));
  m_graphical->warp(AbstractMove::Ptr(), position);
  return true;
}

bool EditPositionController::setStartingPosition() {
  AbstractPosition::Ptr startingPosition = m_variant->createPosition();
  startingPosition->setup();
  std::cout << "adding warp to undo history:" << std::endl;
  std::cout << m_graphical->position()->fen(0, 0) << std::endl;
  std::cout << startingPosition->fen(0, 0) << std::endl;
  m_undo_history.add(WarpAction(m_graphical->position()->clone(), startingPosition));
  m_graphical->warp(AbstractMove::Ptr(), startingPosition);
  return true;
}

QString EditPositionController::fen() {
  return m_graphical->position()->fen(0,0);
}

bool EditPositionController::setFEN(const QString& fen) {
  AbstractPosition::Ptr pos = m_variant->createPositionFromFEN(fen);
  if (pos) {
    std::cout << "warping to fen " << fen << std::endl;
    m_undo_history.add(WarpAction(m_graphical->position()->clone(), pos));
    m_graphical->warp(AbstractMove::Ptr(), pos);
    return true;
  }
  else {
    std::cout << "invalid fen" << std::endl;
    return false;
  }
}

void EditPositionController::setTurn(int turn) {
  m_graphical->setTurn(turn);
}

AbstractPosition::Ptr EditPositionController::currentPosition() const {
  return m_graphical->position();
}

