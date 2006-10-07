/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef EDITPOSITIONCONTROLLER_H
#define EDITPOSITIONCONTROLLER_H

#include "abstract.h"
#include "point.h"
#include "kboard.h"
#include <vector>
#include <boost/variant.hpp>

class VariantInfo;
class GraphicalInfo;
class EditPositionEntity;

//BEGIN UndoAction

class DropAction {
public:
  Point m_to;
  AbstractPiece::Ptr m_piece;
  DropAction(const Point& to, AbstractPiece::Ptr piece)
  : m_to(to)
  , m_piece(piece) { }
};

class RemoveAction {
public:
  Point m_to;
  AbstractPiece::Ptr m_piece;
  RemoveAction(const Point& to, AbstractPiece::Ptr piece)
  : m_to(to)
  , m_piece(piece) { }
};

class MoveAction {
public:
  Point m_from;
  Point m_to;
  AbstractPiece::Ptr m_piece;
  MoveAction(const Point& from, const Point& to, AbstractPiece::Ptr piece)
  : m_from(from)
  , m_to(to)
  , m_piece(piece) { }
};

class WarpAction {
public:
  AbstractPosition::Ptr m_from;
  AbstractPosition::Ptr m_to;
  WarpAction(AbstractPosition::Ptr from, AbstractPosition::Ptr to)
  : m_from(from)
  , m_to(to) { }
};

typedef boost::variant<DropAction, RemoveAction, MoveAction, WarpAction> UndoAction;

class UndoHistory {
  typedef std::vector<UndoAction> Vector;
  Vector m_history;
  uint m_index;
public:
  UndoHistory();
  void add(const UndoAction& action);
  const UndoAction* undo();
  const UndoAction* redo();
};

//END UndoAction

class EditPositionController : public Controller {
  boost::shared_ptr<GraphicalInfo> m_graphical;
  boost::shared_ptr<EditPositionEntity> m_entity;
  UndoHistory m_undo_history;
  VariantInfo* m_variant;
protected:
  virtual boost::shared_ptr<UserEntity> entity() const;
public:
  EditPositionController(ChessTable* view, VariantInfo* variant);
  virtual boost::shared_ptr<Controller> end() { return shared_from_this(); }
  virtual bool clearBoard();
  virtual bool setStartingPosition();
  virtual bool setFEN(const QString&);
  virtual QString fen();

  virtual void setPromotionType(int) { }

  virtual void setTurn(int);

  virtual void handleMoveList(const class PGN&) { }
  virtual QString save() { return ""; }
  virtual void loadPGN(const PGN&) { }

  virtual AbstractPosition::Ptr currentPosition() const;

};

#endif // EDITPOSITIONCONTROLLER_H
