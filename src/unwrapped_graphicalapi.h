/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef UNWRAPPEDGRAPHICALAPI_H
#define UNWRAPPEDGRAPHICALAPI_H

/**
  * This class moves the interface that will be used by the animator to modify
  * kboard graphics into the variant's semantics.
  */
template <typename Variant>
class UnwrappedGraphicalAPI {
  GraphicalAPI* m_graphical_api;
public:
  UnwrappedGraphicalAPI(GraphicalAPI* interface)
    : m_graphical_api(interface) {
  }

  virtual const PointConverter* converter() {
    return m_graphical_api->converter();
  }

  virtual const typename Variant::Position* position() {
    const WrappedPosition<Variant> *p = dynamic_cast<const WrappedPosition<Variant>*>(m_graphical_api->position());
    if(p)
      return p->inner();
    else {
      MISMATCH(m_graphical_api->position(), WrappedPosition<Variant>);
      return NULL;
    }
  }

  virtual SpritePtr setSprite(const Point& p, const typename Variant::Piece& piece, bool use_drop, bool show) {
    WrappedPiece<Variant> wpiece(piece);
    return m_graphical_api->setSprite(p, &wpiece, use_drop, show);
  }

  virtual SpritePtr getSprite(const Point& p) {
    return m_graphical_api->getSprite(p);
  }

  virtual SpritePtr takeSprite(const Point& p) {
    return m_graphical_api->takeSprite(p);
  }

  virtual SpritePtr insertPoolSprite(int index, const AbstractPiece& piece) {
    WrappedPiece<Variant> wpiece(piece);
    return m_graphical_api->insertPoolSprite(index, &wpiece);
  }

  virtual SpritePtr getPoolSprite(int index) {
    return m_graphical_api->getPoolSprite(index);
  }

  virtual SpritePtr takePoolSprite(int index) {
    return m_graphical_api->takePoolSprite(index);
  }
};

#endif //UNWRAPPEDGRAPHICALAPI_H
