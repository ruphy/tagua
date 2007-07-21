/*
  Copyright (c) 2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef ANIMATIONFACTORY_H
#define ANIMATIONFACTORY_H

#include "animation.h"
#include "fwd.h"
#include "point.h"

class NamedSprite;
class PointConverter;
class IndexConverter;
class GraphicalAPI;

namespace Animate {
  
  /**
    * AnimationType distinguishes between those animations which honor settings 
    * and those that are instantaneous regardless of user preferences.
    */
  enum AnimationType {
    Normal,             /// Honor user settings.
    Instant             /// Instantaneous animation.
  };
  
  /**
    * @brief A scheme of animations. 
    * 
    * Animation schemes are descriptions of an animation, i.e. what the animation
    * should suggest to the user. The actual Animation class implementing the scheme
    * depends of various factors, such as the AnimationType that the animator imposes
    * and user settings.
    */
  class Scheme {
  public:
    virtual ~Scheme();
    
    /**
      * Convert the scheme into an actual animation which can be enqueued in the
      * animation system, or grouped into an AnimationGroup.
      */
    virtual AnimationPtr run(const PointConverter*, AnimationType) const = 0;
  };

}

/**
  * @brief A convenience wrapper around an AnimationGroup, useful for Animators.
  * 
  * An AnimationFactory is a wrapper around AnimationGroup that accepts shemes instead
  * of Animations in its addPreAnimation() and addPostAnimation() methods.
  * It offers a convenient syntax to an animator when composing animations into groups.
  */
class AnimationFactory {
  AnimationGroupPtr m_group;
  GraphicalAPI* m_api;
public:
  AnimationFactory(GraphicalAPI* api);
  
  /** \return The wrapped animation group. */
  AnimationGroupPtr group() const;
  
  /** Change the wrapped animation group. */
  void setGroup(const AnimationGroupPtr& group);
  
  /**
    * Add a pre-animation to the group.
    * \param scheme The scheme producing the animation to be added.
    * \param type The AnimationType to be used when creating the animation.
    */
  void addPreAnimation(const Animate::Scheme& scheme, Animate::AnimationType type = Animate::Normal);
  
  /**
    * Add a post-animation to the group.
    * \param scheme The scheme producing the animation to be added.
    * \param type The AnimationType to be used when creating the animation.
    */
  void addPostAnimation(const Animate::Scheme& scheme, Animate::AnimationType type = Animate::Normal);
  
  /** 
    * Implicitly convert the object to an AnimationGroup shared pointer,
    * using the group() member function.
    */
  operator AnimationGroupPtr() const;
};

namespace Animate {
  
  /**
    * @brief A movement animation scheme.
    * 
    * Used to animate the movement of a piece on the board to a destination square.
    */
  class move : public Scheme {
  public:
    enum MovementType {
      Straight = 0x00,
      Rotating = 0x01,
      LShaped = 0x02
    };
  private:
    const NamedSprite& m_sprite;
    Point m_to;
    int m_type;
  public:
    move(const NamedSprite& sprite, const Point& to, int type = Straight);
    virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
  };
  
  /**
    * @brief Appear animation scheme.
    * 
    * The appear scheme visualizes a new piece coming into existence. It is 
    * used, for example, when retracting a capture in chess, to restore the captured
    * piece on the chessboard.
    */
  class appear : public Scheme {
    const NamedSprite& m_sprite;
  public:
    appear(const NamedSprite& sprite);
    virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
  };
  
  /**
    * @brief Disappear animation scheme.
    * 
    * The disappear scheme visualizes a piece being removed from the board.
    */
  class disappear : public Scheme {
    const NamedSprite& m_sprite;
  public:
    disappear(const NamedSprite& sprite);
    virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
  };
  
  /**
    * @brief Animation scheme destroying a piece.
    * 
    * This animation scheme is used to destroy a piece, removing it from the
    * board. It is similar to the disappear scheme, but can provide an additional
    * effect like an explosion.
    */
  class destroy : public Scheme {
    const NamedSprite& m_sprite;
  public:
    destroy(const NamedSprite& sprite);
    virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
  };
  
  /**
    * @brief Animation scheme for changing a piece into another.
    * 
    * The morph animation scheme is used when a piece changes, like in chess promotions.
    */
  class morph : public Scheme {
    const NamedSprite& m_sprite;
    const NamedSprite& m_new_sprite;
  public:
    morph(const NamedSprite& sprite, const NamedSprite& new_sprite);
    virtual AnimationPtr run(const PointConverter* converter, AnimationType type) const;
  };
  
  /**
    * Animations of the pool.
    */
  namespace Pool {
  
    class Scheme {
    public:
      virtual ~Scheme() { }
      virtual AnimationPtr run(const IndexConverter* converter, AnimationType) const = 0;
    };
    
    class move : public Scheme {
      const NamedSprite& m_sprite;
      int m_to;
    public:
      move(const NamedSprite& sprite, int to);
      virtual AnimationPtr run(const IndexConverter*, AnimationType type) const;
    };
    
    class appear : public Scheme {
      const NamedSprite& m_sprite;
    public:
      appear(const NamedSprite& sprite);
      virtual AnimationPtr run(const IndexConverter*, AnimationType type) const;
    };
    
    class disappear : public Scheme {
      const NamedSprite& m_sprite;
    public:
      disappear(const NamedSprite& sprite);
      virtual AnimationPtr run(const IndexConverter*, AnimationType type) const;
    };
  }


}


#endif // ANIMATIONFACTORY_H
