/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef INFODISPLAY_H
#define INFODISPLAY_H

#include "canvas/group.h"

class MessageItem;

class InfoDisplay : public Canvas::Group {
  MessageItem* m_message;
protected:
  void resizeEvent(QResizeEvent*);
public:
  InfoDisplay(Canvas::Abstract* parent = 0);
  ~InfoDisplay();
  void setMessage(const QString&);
  void settingsChanged();
};

#endif // INFODISPLAY_H
