/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "highlevel.h"
#include "graphicalposition.h"

template <typename Variant>
class WrappedElement {
  Element m_inner;
public:
  typedef boost::shared_ptr<PieceSprite> SpritePtr;
  
  Element inner() const { return m_inner; }
  operator bool() const { return m_inner; }

  WrappedElement() { }
  WrappedElement(const Element& element)
  : m_inner(element) {
  	if (m_inner.piece())
			TYPECHECK(*m_inner.piece(), WrappedPiece<Variant>);
//     if (!dynamic_cast<WrappedPiece<Variant>*>(m_inner.piece().get()))
//       MISMATCH(m_inner.piece().get(), WrappedPiece<Variant>);
  }
  
  const typename Variant::Piece& piece() const {
    WrappedPiece<Variant>* res = dynamic_cast<WrappedPiece<Variant>*>(m_inner.piece().get());
    Q_ASSERT(res);
    return res->inner(); 
  }
  
  SpritePtr sprite() const { return m_inner.sprite(); }
};

template <typename Variant>
class GenericGraphicalPosition {
  typedef boost::shared_ptr<PieceSprite> SpritePtr;
  typedef typename Variant::Piece Piece;
public:
  virtual ~GenericGraphicalPosition() { }
  virtual int getIntSetting(const QString& key, int def_value) const = 0;
  virtual bool getBoolSetting(const QString& key, bool def_value) const = 0;
  virtual QString getStringSetting(const QString& key, const QString& def_value) const = 0;

  virtual SpritePtr getSprite(const Point& p) const = 0;

  virtual SpritePtr setPiece(const Point& p, const Piece& e,
                             bool usedrop = false, bool show = false) = 0;

  virtual WrappedElement<Variant> getElement(const Point& p) const = 0;
  
  virtual void setElement(const Point& p, const WrappedElement<Variant>& e) = 0;
  
  virtual void removeElement(const Point& p) = 0;
  virtual void updatePool(AbstractPosition::PoolPtr pool) = 0;
  virtual void addTag(const QString& name, Point, bool over = false) = 0;
  virtual void clearTags(const QString& name) = 0;
  virtual void setTags(const QString& name, Point p1 = Point::invalid(), Point p2 = Point::invalid(),
                                      Point p3 = Point::invalid(), Point p4 = Point::invalid(),
                                      Point p5 = Point::invalid(), Point p6 = Point::invalid() ) = 0;
  
  virtual Point first() const = 0;
  virtual Point last() const = 0;
  virtual Point next(const Point& p) const = 0;
  virtual bool valid(const Point& p) const = 0;
  virtual bool consistent() const = 0;
};

/**
	* @brief A strongly typed wrapper aroung GraphicalPosition.
  */
template <typename Variant>
class WrappedGraphicalPosition : public GenericGraphicalPosition<Variant> {
  typedef boost::shared_ptr<PieceSprite> SpritePtr;
  typedef typename Variant::Piece Piece;
  GraphicalPosition* m_inner;
public:
  typedef WrappedElement<Variant> GElement;
  WrappedGraphicalPosition(GraphicalPosition* inner)
  : m_inner(inner) { }

  virtual int getIntSetting(const QString& key, int def_value) const {
    return m_inner->getIntSetting(key, def_value);
  }
  
  virtual bool getBoolSetting(const QString& key, bool def_value) const {
    return m_inner->getBoolSetting(key, def_value);
  }
  
  virtual QString getStringSetting(const QString& key, const QString& def_value) const {
    return m_inner->getStringSetting(key, def_value);
  }

  virtual SpritePtr getSprite(const Point& p) const {
    return m_inner->getSprite(p);
  }

  virtual SpritePtr setPiece(const Point& p, const Piece& e,
                             bool usedrop = false, bool show = false) {
    AbstractPiece::Ptr piece(new WrappedPiece<Variant>(e));
    return m_inner->setPiece(p, piece, usedrop, show);
  }

  virtual GElement getElement(const Point& p) const {
    return m_inner->getElement(p);
  }
  
  virtual void setElement(const Point& p, const GElement& e) {
    m_inner->setElement(p, e.inner());
  }
  
  virtual void removeElement(const Point& p) { m_inner->removeElement(p); }
  virtual void updatePool(AbstractPosition::PoolPtr pool) { m_inner->updatePool(pool); }
  virtual void addTag(const QString& name, Point p, bool over = false) { m_inner->addTag(name, p, over); }
  virtual void clearTags(const QString& name) { m_inner->clearTags(name); }
  virtual void setTags(const QString& name, Point p1 = Point::invalid(), Point p2 = Point::invalid(),
                                      Point p3 = Point::invalid(), Point p4 = Point::invalid(),
                                      Point p5 = Point::invalid(), Point p6 = Point::invalid() ) {
    m_inner->setTags(name, p1, p2, p3, p4, p5, p6);
  }
  
  virtual Point first() const { return m_inner->first(); }
  virtual Point last() const { return m_inner->last(); }
  virtual Point next(const Point& p) const { return m_inner->next(p); }
  virtual bool valid(const Point& p) const { return m_inner->valid(p); }
  virtual bool consistent() const { return m_inner->consistent(); }
};


