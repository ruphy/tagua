/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QResizeEvent>
#include "infodisplay.h"
#include "canvas/text.h"

class MessageItem : public Canvas::Text {
public:
  MessageItem(const QString& text, const QFont& font, Canvas::Abstract* canvas)
  : Canvas::Text(text, Qt::black, font,
                 Canvas::Text::HStart, Canvas::Text::VCenter,
                 canvas) { }
};

InfoDisplay::InfoDisplay(Canvas::Abstract* parent)
: Canvas::Group(parent) {
  m_message = new MessageItem("", QFont(), this);
  m_message->moveTo(QPoint(0, 0));
  m_message->show();
}

InfoDisplay::~InfoDisplay() {
  delete m_message;
}

void InfoDisplay::resizeEvent(QResizeEvent* e) {
//  m_message->setSize(e->size());
}

void InfoDisplay::setMessage(const QString& msg) {
  m_message->setText(msg);
}

void InfoDisplay::settingsChanged() {
}
