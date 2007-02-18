/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/


#include "rubyutils.h"
#include "rubyvariant.h"
#include "kboard.h"
#include "xchess/animator.h"
#include "xchess/piece.h"
#include "xchess/move.h"
#include "highlevel.h"
#include "moveserializer.impl.h"
#include "crazyhouse_p.h"
#include "piecefunction.h"

class RubyPosition;

int value2int(VALUE val) {
	if (rb_respond_to(val, rb_intern("to_i")))
		return NUM2INT(rb_funcall(val, rb_intern("to_i"), 0));
	else
		return static_cast<int>(SYM2ID(val));
}

class RubyPiece : public AbstractPiece {
  VALUE m_piece; 
public:
  RubyPiece(VALUE);
  
  VALUE inner() const { return m_piece; }
  
  virtual bool equals(AbstractPiece::Ptr other) const;
  virtual bool less(AbstractPiece::Ptr other) const;
  virtual QString name() const;
  virtual AbstractPiece::Ptr clone() const;
  virtual int color() const;
  virtual int type() const;
};

class RubyMove : public AbstractMove {
  VALUE m_move;
public:
  RubyMove(VALUE move) : m_move(move) { }
  
  VALUE inner() const { return m_move; }
  
	virtual QString SAN(AbstractPosition::Ptr ref) const;
	virtual DecoratedMove toDecoratedMove(AbstractPosition::Ptr) const;
	virtual QString toString(AbstractPosition::Ptr) const;
	virtual NormalUserMove toUserMove() const;
	virtual bool equals(AbstractMove::Ptr) const;
};

class RubyPool {
public:
  RubyPool(VALUE);
};

class RubyVariantInfo : public VariantInfo {
  VALUE m_type;
  VALUE m_color;
  VALUE m_piece;
  VALUE m_move;
  VALUE m_position;
  
  QString m_name;
public:
  VALUE typeValue(int type);
  VALUE colorValue(int type);

  virtual AbstractPosition::Ptr createPosition();
  virtual AbstractPosition::Ptr createCustomPosition(const OptList& l);

  virtual AbstractPosition::Ptr createPositionFromFEN(const QString& fen);

  virtual AbstractPosition::Ptr createChessboard(int turn,
                              bool wk, bool wq, bool bk, bool bq,
                              const Point& ep);
                              
  virtual AbstractPiece::Ptr createPiece(int color, int type);
  virtual void forallPieces(class PieceFunction& f);
  virtual int moveListLayout() const;
  virtual QStringList borderCoords() const;

  virtual AbstractAnimator::Ptr createAnimator(PointConverter* converter,
                                           GraphicalPosition* position);
  virtual AbstractMove::Ptr createNormalMove(const NormalUserMove& move);
  virtual AbstractMove::Ptr createDropMove(const DropUserMove& move);

  virtual AbstractMove::Ptr getVerboseMove(int turn, const VerboseNotation& m) const;
  virtual int type(const QString& str);
  virtual QString typeSymbol(int type);
  virtual bool simpleMoves();
  virtual QString name() const;
  virtual QString themeProxy() const;
  virtual OptList positionOptions() const;
};

class RubyAnimator : public AbstractAnimator {
	typedef boost::shared_ptr<AnimationGroup> AnimationPtr;
public:
	RubyAnimator() { }
  virtual AnimationPtr warp(AbstractPosition::Ptr) { return AnimationPtr(); }

  virtual AnimationPtr forward(AbstractPosition::Ptr, AbstractMove::Ptr) { return AnimationPtr(); }
  virtual AnimationPtr back(AbstractPosition::Ptr, AbstractMove::Ptr) { return AnimationPtr(); }
};

static VALUE pool_set_helper(VALUE arg, VALUE _pool) {
  AbstractPosition::AbstractPool* pool;
  Data_Get_Struct(_pool, AbstractPosition::AbstractPool, pool);
  VALUE piece = rb_ary_entry(arg, 0);
  int count = NUM2INT(rb_ary_entry(arg, 1));
  (*pool)[AbstractPiece::Ptr(new RubyPiece(piece))] = count;
  
  return Qnil;
}

static VALUE execute_piece_function(VALUE arg, VALUE functor) {
  PieceFunction* f;
  Data_Get_Struct(functor, PieceFunction, f);
  RubyPiece piece(arg);
  (*f)(piece.color(), piece.type());
  return Qnil;
}


RubyPiece::RubyPiece(VALUE piece)
: m_piece(piece) { }

bool RubyPiece::equals(AbstractPiece::Ptr _other) const {
  if (!_other) return false;
  RubyPiece* other = dynamic_cast<RubyPiece*>(_other.get());

  if (other)
    return RTEST(rb_funcall(m_piece, rb_intern("=="), 1, other->inner()));
  else {
    MISMATCH(*_other.get(),RubyPiece);
    return false;
  }
}

bool RubyPiece::less(AbstractPiece::Ptr _other) const {
  if (!_other) return false;
  RubyPiece* other = dynamic_cast<RubyPiece*>(_other.get());

  if (other)
    return RTEST(rb_funcall(m_piece, rb_intern("<="), 1, other->inner()));
  else {
    MISMATCH(*_other.get(),RubyPiece);
    return false;
  }
}

AbstractPiece::Ptr RubyPiece::clone() const {
  VALUE cl = rb_funcall(m_piece, rb_intern("clone"), 0);
  return AbstractPiece::Ptr(new RubyPiece(cl));
}

QString RubyPiece::name() const {
  VALUE name_value = rb_funcall(m_piece, rb_intern("to_s"), 0);
  return StringValuePtr(name_value); 
}

int RubyPiece::color() const {
  VALUE color_value = rb_funcall(m_piece, rb_intern("color"), 0);
	return value2int(color_value);
}

int RubyPiece::type() const {
  if (rb_respond_to(m_piece, rb_intern("type"))) {
    VALUE type_value = rb_funcall(m_piece, rb_intern("type"), 0);
		return value2int(type_value);
  }
  else return -1;
}

class RubyPosition : public AbstractPosition {
  typedef RubyMove Move;
  typedef RubyPiece Piece;

  VALUE m_pos;
  RubyVariantInfo* m_variant;
public:
  VALUE& inner() { return m_pos; }
  const VALUE& inner() const { return m_pos; }

  RubyPosition(const VALUE& pos, RubyVariantInfo* variant)
  : m_pos(pos)
  , m_variant(variant) { }

  virtual QString variant() const {
    return m_variant->name();
  }

  virtual Point size() const {
    VALUE res = rb_funcall(m_pos, rb_intern("size"), 0);
    return Ruby::value2point(res);
  }

  virtual PoolPtr pool() const {
    AbstractPool* p = new AbstractPool;
    VALUE pool = rb_funcall(m_pos, rb_intern("pool"), 0);
    VALUE c_pool = Data_Wrap_Struct(rb_cObject, 0, 0, p);
    rb_iterate(rb_each, pool, (VALUE(*)(...))pool_set_helper, c_pool); 
    return PoolPtr(p);
  }

  virtual void addToPool(AbstractPiece::Ptr _piece, int n) {
    RubyPiece* piece = dynamic_cast<RubyPiece*>(_piece.get());
    VALUE pool = rb_funcall(m_pos, rb_intern("pool"), 0);

    if (piece)
      rb_funcall(pool, rb_intern("[]="), 2, piece->inner(), INT2FIX(n));
    else
      MISMATCH(*_piece.get(),RubyPiece);
  }

  virtual void removeFromPool(AbstractPiece::Ptr _piece, int n) {
    RubyPiece* piece = dynamic_cast<RubyPiece*>(_piece.get());
    VALUE pool = rb_funcall(m_pos, rb_intern("pool"), 0);
    
    if (piece) {
      VALUE count_value = rb_funcall(pool, rb_intern("[]"), 1, piece->inner());
      int count = 0;
      if (RTEST(count_value))
        count = NUM2INT(count_value) - n;
      if (count <= 0)
        rb_funcall(pool, rb_intern("delete"), 1, piece->inner());
      else
        rb_funcall(pool, rb_intern("[]="), 2, piece->inner(), count);
    }
    else
      MISMATCH(*_piece.get(),RubyPiece);
  }

  virtual void copyPoolFrom(AbstractPosition::Ptr _other) {
    RubyPosition* other = dynamic_cast<RubyPosition*>(_other.get());

    if (other) {
      VALUE other_pool = rb_funcall(other->inner(), rb_intern("pool"), 0);
      rb_funcall(m_pos, rb_intern("pool="), 1, other_pool);
    }
    else
      MISMATCH(*_other.get(), RubyPosition);
  }

  virtual void setPool(PoolPtr new_pool) {
    VALUE pool = rb_funcall(m_pos, rb_intern("pool"), 0);
    rb_funcall(pool, rb_intern("clear"), 0);

    for (AbstractPool::iterator it = new_pool->begin(); it != new_pool->end(); ++it) {
      RubyPiece* piece = dynamic_cast<RubyPiece*>(it->first.get());

      if (piece)
        rb_funcall(pool, rb_intern("[]="), 2, piece->inner(), NUM2INT(it->second));
      else {
        MISMATCH(*it->first.get(), RubyPiece);
        return;
      }
    }
  }

  virtual AbstractPiece::Ptr get(const Point& p) const {
    VALUE piece = rb_funcall(m_pos, rb_intern("[]"), 1, Ruby::point2value(p));
    if (piece)
      return AbstractPiece::Ptr(new RubyPiece(piece));
    else
      return AbstractPiece::Ptr();
  }

  virtual void set(const Point& p, AbstractPiece::Ptr _piece) {
    if (!_piece)
      rb_funcall(m_pos, rb_intern("[]="), 2, Ruby::point2value(p), Qnil);
    else {
      RubyPiece* piece = dynamic_cast<RubyPiece*>(_piece.get());

      if (piece)
        rb_funcall(m_pos, rb_intern("[]="), 2, Ruby::point2value(p), piece->inner());
      else
        MISMATCH(*_piece.get(),RubyPiece);
    }
  }

  virtual int turn() const {
    VALUE turn_value = rb_funcall(m_pos, rb_intern("turn"), 0);
		return value2int(turn_value);
  }

  virtual void setTurn(int turn) {
    VALUE turn_value = m_variant->colorValue(turn);
    rb_funcall(m_pos, rb_intern("turn="), 1, turn_value);
  }

  virtual int previousTurn() const {
    VALUE turn_value = rb_funcall(m_pos, rb_intern("previous_turn"), 0);
    return value2int(turn_value);
  }

  virtual void switchTurn() {
    rb_funcall(m_pos, rb_intern("switch_turn"), 0);
  }

  virtual bool testMove(AbstractMove::Ptr _move) const {
    RubyMove* move = dynamic_cast<RubyMove*>(_move.get());

    if (move)
      return RTEST(rb_funcall(m_pos, rb_intern("test_move"), 1, move->inner()));
    else {
      MISMATCH(*_move.get(),RubyMove);
      return false;
    }
  }

  virtual void move(AbstractMove::Ptr _move) {
    RubyMove* move = dynamic_cast<RubyMove*>(_move.get());

    if (move)
      rb_funcall(m_pos, rb_intern("move"), 1, move->inner());
    else
      MISMATCH(*_move.get(),RubyMove);
  }

  virtual void setup() {
    rb_funcall(m_pos, rb_intern("setup"), 0);
  }

  virtual AbstractPosition::Ptr clone() const {
    VALUE cl = rb_funcall(m_pos, rb_intern("clone"), 0);
    return AbstractPosition::Ptr(new RubyPosition(cl, m_variant));
  }

  virtual bool equal(AbstractPosition::Ptr _other) const {
    RubyPosition* other = dynamic_cast<RubyPosition*>(_other.get());

    if(other)
      return RTEST(rb_funcall(m_pos, rb_intern("=="), 1, other->inner()));
    else {
      MISMATCH(*_other.get(),RubyPosition);
      return false;
    }
  }

  virtual AbstractMove::Ptr getMove(const AlgebraicNotation&) const {
    return AbstractMove::Ptr();
  }

  virtual AbstractMove::Ptr getMove(const QString&) const {
    return AbstractMove::Ptr();
  }

  virtual QString state() const {
    return ""; // TODO
  }
  
	virtual QStringList borderCoords() const {
		return QStringList(); //TODO
	}

  virtual QString fen(int /*halfmove*/, int /*fullmove*/) const {
    return "";
  }

  virtual AbstractPiece::Ptr moveHint(AbstractMove::Ptr) const {
    // TODO
    return AbstractPiece::Ptr();
  }

  virtual void dump() const {
    if (rb_respond_to(m_pos, rb_intern("to_s"))) {
      VALUE dumped_value = rb_funcall(m_pos, rb_intern("to_s"), 0);
      const char* dumped = StringValuePtr(dumped_value);
      std::cout << wrap_cptr(dumped) << std::endl;
    }
  }
};


QString RubyMove::SAN(AbstractPosition::Ptr ref) const {
	if (RubyPosition* pos = dynamic_cast<RubyPosition*>(ref.get())) {
		VALUE res = rb_funcall(m_move, rb_intern("san"), 1, pos->inner());
		return StringValuePtr(res);
	}
	else {
		MISMATCH(*ref.get(), RubyPosition);
		return "";
	}
}

DecoratedMove RubyMove::toDecoratedMove(AbstractPosition::Ptr ref) const {
	// TODO
	DecoratedMove res;
	QString san = SAN(ref);
	res.push_back(MovePart(san));
	return res;
}
	
QString RubyMove::toString(AbstractPosition::Ptr ref) const {
	if (RubyPosition* pos = dynamic_cast<RubyPosition*>(ref.get())) {
		VALUE res = rb_funcall(m_move, rb_intern("to_s"), 1, pos->inner());
		return StringValuePtr(res);
	}
	else {
		MISMATCH(*ref.get(), RubyPosition);
		return "";
	}
}

NormalUserMove RubyMove::toUserMove() const {
	VALUE res = rb_funcall(m_move, rb_intern("to_user"), 0);
	Point from = Ruby::value2point(rb_funcall(res, rb_intern("from"), 0));
	Point to = Ruby::value2point(rb_funcall(res, rb_intern("to"), 0));
	return NormalUserMove(from, to);
}
	
bool RubyMove::equals(AbstractMove::Ptr other) const {
	if (RubyMove* mv = dynamic_cast<RubyMove*>(other.get())) {
		return RTEST(rb_funcall(m_move, rb_intern("=="), 1, mv->inner()));
	}
	else {
		MISMATCH(*other, RubyMove);
		return false;
	}
}


VALUE RubyVariantInfo::typeValue(int type) {
	if (rb_respond_to(m_type, rb_intern("from_i")))
		return rb_funcall(m_type, rb_intern("from_i"), 1, type);
	else
		return ID2SYM(static_cast<ID>(type));
}

VALUE RubyVariantInfo::colorValue(int color) {
	if (rb_respond_to(m_type, rb_intern("from_i")))
		return rb_funcall(m_type, rb_intern("from_i"), 1, color);
	else
		return ID2SYM(static_cast<ID>(color));
}

AbstractPosition::Ptr RubyVariantInfo::createPosition() {
  VALUE pos = rb_funcall(m_position, rb_intern("new"), 0);
  return AbstractPosition::Ptr(
    new RubyPosition(pos, this));
}

AbstractPosition::Ptr RubyVariantInfo::createCustomPosition(const OptList&) {
  // TODO
  return createPosition();
}

AbstractPosition::Ptr RubyVariantInfo::createPositionFromFEN(const QString& fen) {
  if (rb_respond_to(m_position, rb_intern("from_fen"))) {
//     int ok;
    VALUE pos = rb_funcall(m_position, rb_intern("from_fen"), 1, rb_str_new2(fen.toAscii()));
//     if (!ok) return AbstractPosition::Ptr();
    return AbstractPosition::Ptr(new RubyPosition(pos, this));
  }
  
  return AbstractPosition::Ptr();
}

AbstractPosition::Ptr RubyVariantInfo::createChessboard(int /*turn*/,
                            bool /*wk*/, bool /*wq*/, bool /*bk*/, bool /*bq*/,
                            const Point& /*ep*/) {
  return AbstractPosition::Ptr();
}

AbstractPiece::Ptr RubyVariantInfo::createPiece(int color, int type) {
  if (rb_respond_to(m_piece, rb_intern("from_color_type"))) {
  	VALUE piece = rb_funcall(m_piece, rb_intern("from_color_type"), 2, 
  		colorValue(color), typeValue(type));
		return AbstractPiece::Ptr(new RubyPiece(piece));
  }
  else
  	return AbstractPiece::Ptr();
}

void RubyVariantInfo::forallPieces(PieceFunction& f) {
  VALUE functor = Data_Wrap_Struct(rb_cObject, 0, 0, &f);
  VALUE piece_class = rb_funcall(m_piece, rb_intern("class"), 0);
  rb_iterate(rb_each, piece_class, (VALUE(*)(...))execute_piece_function, functor);
}

int RubyVariantInfo::moveListLayout() const {
//   if (rb_respond_to(m_variant, rb_intern("movelist_layout")))
//     return NUM2INT(rb_funcall(m_variant, rb_intern("movelist_layout")));
//   else
    return 0;
}

QStringList RubyVariantInfo::borderCoords() const {
  // TODO
  return QStringList() << "a" << "b" << "c" << "d" << "e" << "f" << "g" << "h"
                      << "1" << "2" << "3" << "4" << "5" << "6" << "7" << "8";
}

AbstractAnimator::Ptr RubyVariantInfo::createAnimator(PointConverter* /*converter*/,
                                          GraphicalPosition* /*position*/) {
	// TODO
  return AbstractAnimator::Ptr(new RubyAnimator);
}

AbstractMove::Ptr RubyVariantInfo::createNormalMove(const NormalUserMove& move) {
	VALUE mv = rb_funcall(m_move, rb_intern("from_normal"), Ruby::point2value(move.from), Ruby::point2value(move.to));
  return AbstractMove::Ptr(new RubyMove(mv));
}

AbstractMove::Ptr RubyVariantInfo::createDropMove(const DropUserMove& move) {
  RubyPiece* piece = dynamic_cast<RubyPiece*>(move.m_piece.get());
  if (piece) {
  	VALUE mv = rb_funcall(m_move, rb_intern("from_drop"), piece->inner(), Ruby::point2value(move.m_to));
    return AbstractMove::Ptr(new RubyMove(mv));
  }
  else {
    MISMATCH(move.m_piece.get(), RubyPiece);
    return AbstractMove::Ptr();
  }
}

AbstractMove::Ptr RubyVariantInfo::getVerboseMove(int /*turn*/, const VerboseNotation& /*m*/) const {
//   Move res = Position::getVerboseMove(static_cast<typename Piece::Color>(turn), m);
//   return AbstractMove::Ptr(new RubyMove(res));
	// TODO
	return AbstractMove::Ptr(new RubyMove(Qnil));
}

int RubyVariantInfo::type(const QString& str) {
	VALUE type_str = rb_str_new2(str.toAscii());
	if (rb_respond_to(m_type, rb_intern("from_s"))) {
  	VALUE res = rb_funcall(m_type, rb_intern("from_s"), type_str);
  	return value2int(res);
	}
	else
		return -1;
}

QString RubyVariantInfo::typeSymbol(int type) {
	VALUE type_value = typeValue(type);
	if (rb_respond_to(type_value, rb_intern("symbol"))) {
		VALUE res = rb_funcall(type_value, rb_intern("symbol"), 0);
		return StringValuePtr(res);
	}
	else
		return "";
}

bool RubyVariantInfo::simpleMoves() {
	//TODO
  return false;
}

QString RubyVariantInfo::name() const {
  return m_name;
}
QString RubyVariantInfo::themeProxy() const {
  return "";
}
OptList RubyVariantInfo::positionOptions() const {
  return OptList();
}

