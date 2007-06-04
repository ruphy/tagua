/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVELIST_TEXTUAL_H
#define MOVELIST_TEXTUAL_H

#include <vector>
#include <QWidget>
#include <khtml_part.h>
#include "decoratedmove.h"
#include "spriteloader.h"
#include "movelist_notifier.h"

class KHTMLPart;
class KUrl;

namespace MoveList {

class Textual : public KHTMLPart {
Q_OBJECT
  Index m_curr_selected;
  int   m_layout_style;
  Notifier *m_notifier;
  SpriteLoader m_loader;

public:
  Textual(QWidget *parent = NULL);

  void settingsChanged() {}

  void setLoaderBasePath(const QString& p){ m_loader.setBasePath(p); }

  /** Sets the move comment at the given index */
  void setComment(const Index& index, const QString& comment);

  /** Sets the comment at the given index before the give subvariation */
  void setVComment(const Index& index, int v, const QString& comment);

  /** Sets the move at the given index */
  void setMove(const Index& index, int turn, const DecoratedMove& move,
                                  const QString& comment = QString());

  /** Sets the move at the given index */
  void setMove(const Index& index, int turn, const QString& move,
                                  const QString& comment = QString());

  /** Removes the given index and all those that come after */
  void remove(const Index& index);

  void reset();

  Notifier* getNotifier();

  void setNotifier(Notifier* n, bool detach_prev=true);

  int layoutStyle();

  void setLayoutStyle(int x);

  /** Sets the currently selected index */
  void select(const Index& index);

private slots:
  void onURL(const KUrl &url);
};

} //end namespace MoveList

#endif //MOVELIST_TEXTUAL_H
