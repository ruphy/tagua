/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef __MOVELIST_TABLE_H__
#define __MOVELIST_TABLE_H__

#include <QWidget>

class QAction;
class QScrollArea;

namespace MoveList {
  class Widget;

  /**
    * @class Table <movelist_table.h>
    * @brief The widget that holds the move list in a scrolled view.
    *
    * This class is a widget that contains a MoveList::Widget and a few
    * convenience buttons for undo and redo.
    */
  class Table : public QWidget {
  Q_OBJECT
  public:
    /** the action for undo */
    QAction *m_undo;

    /** the action for redo */
    QAction *m_redo;

    /** the scroll area that contains the move list */
    QScrollArea *m_scroll_area;

    /** the move list widget */
    Widget *m_movelist;

    Table(QWidget* p = NULL);

  private slots:
    void onUndo();
    void onRedo();
  };
} //end namespace MoveList

#endif //__MOVELIST_TABLE_H__
