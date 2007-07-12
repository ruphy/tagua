/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MOVELISTWIDGET_H
#define MOVELISTWIDGET_H

#include <map>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <QWidget>
#include <QHash>
#include <map>
#include <vector>
#include "index.h"
#include "decoratedmove.h"
#include "spriteloader.h"
#include "kgamecanvas.h"

namespace MoveList {

typedef boost::shared_ptr<class Entry> EntryPtr;
typedef boost::shared_ptr<class Brace> BracePtr;
typedef boost::shared_ptr<class Text> TextPtr;
typedef boost::shared_ptr<class Comment> CommentPtr;
typedef std::vector<EntryPtr> History;
typedef std::map<int, History> Variations;
typedef std::map<int, BracePtr> Braces;
typedef std::map<int, CommentPtr> VComments;

class Notifier;
class Table;
class Settings;

/**
  * @class Widget <movelist_widget.h>
  * @brief The widget can be used to display a tree of moves.
  *
  * This class is a widget that can be used to display a tree of moves
  */
class Widget : public KGameCanvasWidget {
private:
  Q_OBJECT
  friend class FancyItem;
  friend class Entry;
  friend class Brace;
  friend class Text;
  friend class Comment;

  friend class Table;

  History      history;

  int entry_size;

  Index curr_highlight;
  int   curr_highlight_type;
  Index curr_selected;

  boost::weak_ptr<Entry> edited_comment;
  int                    edited_comment_variation;
  class QTextEdit*       comment_editor;

  int  layout_max_width;
  bool layout_pending;
  int  layout_style;
  int  layout_time;
  bool layout_goto_selected;
  bool layout_width_changed;
  bool layout_must_relayout;

  Notifier *notifier;
  QHash<QString, QPixmap> loaded_pixmaps;
  Table *owner_table;
  Settings *m_settings;
  SpriteLoader m_loader;

  History* fetchRef(const Index& ix, int* idx = NULL);
  EntryPtr fetch(const Index& ix);

  bool eventFilter(QObject *obj, QEvent *event);
  void startEditing(const Index& ix, int v);
  void stopEditing();

  void layout();
  int layoutHistory(History& array, int at_x, int at_y, int prev_turn, int mv_num,
                                                      int sub_mv_num, bool visible);

  void fixIndices(const Index& ix);
  void setComment(EntryPtr e, int v, const QString& comment);

  QPixmap getPixmap(const QString& s, bool selected = false);

  virtual void resizeEvent ( QResizeEvent * event );
  virtual void mouseMoveEvent ( QMouseEvent * event );
  virtual void mousePressEvent ( QMouseEvent * event );
  virtual void mouseReleaseEvent ( QMouseEvent * event );


private slots:
  void doLayout();

public:
  Widget(QWidget *parent = NULL, Table *o = NULL);
  virtual ~Widget();

  Notifier* getNotifier();
  void setNotifier(Notifier* n, bool detach_prev=true);

  void settingsChanged();

  void setLoaderBasePath(const QString& p){ m_loader.setBasePath(p); }

  /** Clears all the moves */
  void reset();

  /** returns the style of layout */
  int layoutStyle(){ return layout_style; }

  /** sets the style of layout. 0 mean the the moves are paired, like in chess.
      any value x>0 will layout the moves in x columns for each player (useful
      for progressive chess, for instance) */
  void setLayoutStyle(int x) {
    layout_style = x;
    layout();
  }

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

  /** Promotes the given variation in the main line */
  void promoteVariation(const Index& ix, int v);

  /** Sets the currently selected index */
  void select(const Index& index);
};

} //end namespace

#endif
