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

#ifdef HAVE_KHTML

#include <vector>
#include <QWidget>
#include <khtml_part.h>
#include "decoratedmove.h"
#include "pixmaploader.h"
#include "movelist_notifier.h"

class KHTMLPart;
class KUrl;
class ThemeInfo;

namespace MoveList {

class Textual : public KHTMLPart {
Q_OBJECT
  Index m_curr_selected;
  int   m_layout_style;
  Notifier *m_notifier;
  PixmapLoader m_loader;

public:
  Textual(QWidget *parent = NULL);

  void settingsChanged() {}

  void setLoaderTheme(const ThemeInfo& theme) { m_loader.setTheme(theme); }

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

#else //HAVE_KHTML

#include <QWidget>
#include "decoratedmove.h"
#include "index.h"

class ThemeInfo;

namespace MoveList {

class Notifier;

class Textual : public QWidget {
Q_OBJECT

public:
  Textual(QWidget *parent = NULL) : QWidget(parent){}

  void settingsChanged() {}
  void setLoaderTheme(const ThemeInfo& /*theme*/){}
  void setComment(const Index& /*index*/, const QString& /*comment*/){}
  void setVComment(const Index& /*index*/, int /*v*/, const QString& /*comment*/){}
  void setMove(const Index& /*index*/, int /*turn*/, const DecoratedMove& /*move*/,
                                  const QString& /*comment*/ = QString()){}
  void setMove(const Index& /*index*/, int /*turn*/, const QString& /*move*/,
                                  const QString& /*comment*/ = QString()){}
  void remove(const Index& /*index*/){}
  void reset(){}
  Notifier* getNotifier(){ return NULL; }
  void setNotifier(Notifier* /*n*/, bool /*detach_prev*/=true){}
  int layoutStyle(){ return 0; }
  void setLayoutStyle(int /*x*/){}
  void select(const Index& /*index*/){}
};

} //end namespace MoveList

#endif //HAVE_KHTML

#endif //MOVELIST_TEXTUAL_H
