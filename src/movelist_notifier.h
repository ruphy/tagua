/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef __MOVELIST_NOTIFIER_H__
#define __MOVELIST_NOTIFIER_H__

namespace MoveList {
  class Widget;
  class Table;

  /**
    * @class Notifier <movelist_notifier.h>
    * @brief An observer class for user actions on the movelist.
    *
    * This class can be registered to a MoveList::Widget to be notified
    * of user interactions with the move list.
    */
  class Notifier {
  public:
    virtual ~Notifier(){}

    /** the user clicked on a move */
    virtual void onUserSelectMove(const Index&) = 0;

    /** the user changed a comment */
    virtual void onUserSetComment(const Index&, QString) = 0;

    /** the user changed a comment at the start of a variation */
    virtual void onUserSetVComment(const Index&, int v, QString) = 0;

    /** the user wants to clear all the subvariations of the given index */
    virtual void onUserClearVariations(const Index&) = 0;

    /** the user wants to truncate the game at the given index */
    virtual void onUserTruncate(const Index&) = 0;

    /** the user wants to clear all the subvariations of the given entry */
    virtual void onUserPromoteVariation(const Index&) = 0;

    /** the user wants to remove the subvariations containing the given entry */
    virtual void onUserRemoveVariation(const Index&) = 0;

    /** the user wants to undo last change */
    virtual void onUserUndo() = 0;

    /** the user wants to redo last change */
    virtual void onUserRedo() = 0;

    /** this notifier has been kicked off by the movelist */
    virtual void onDetachNotifier() = 0;
  };
}

#endif //__MOVELIST_NOTIFIER_H__
