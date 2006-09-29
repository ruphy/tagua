/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>
            
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QWidget>
#include "flash.h"

#if defined(Q_WS_X11) && defined(HAVE_X11)

#include <QX11Info>
#include <X11/Xutil.h>


void Flash::flash(QWidget* widget) {
  // thanks to Marcin Jakubowski for this code
  // (taken from http://www.qtforum.org/post/52724/lastpost.html#post52724)
  
  Display *xdisplay = QX11Info::display();
  Window rootwin = QX11Info::appRootWindow();
  Window winId = widget->winId();
  
  static Atom demandsAttention = XInternAtom(xdisplay, "_NET_WM_STATE_DEMANDS_ATTENTION", true);
  static Atom wmState = XInternAtom(xdisplay, "_NET_WM_STATE", true);

  XEvent e;
  e.xclient.type = ClientMessage;
  e.xclient.message_type = wmState;
  e.xclient.display = xdisplay;
  e.xclient.window = winId;
  e.xclient.format = 32;
  e.xclient.data.l[0] = 1;
  e.xclient.data.l[1] = demandsAttention;
  e.xclient.data.l[2] = 0l;
  e.xclient.data.l[3] = 0l;
  e.xclient.data.l[4] = 0l;
  
  XSendEvent(xdisplay, rootwin, False, (SubstructureRedirectMask | SubstructureNotifyMask), &e);
}

#elif defined(Q_WS_WIN)

#include <windows.h>

void Flash::flash(QWidget* widget) {
#if 0
  FLASHWINFO fi;
  fi.cbSize = sizeof(FLASHWINFO);
  fi.hwnd = widget->winId(); 
  fi.dwFlags = FLASHW_TRAY;
  fi.uCount = 3;
  fi.dwTimeout = 0;

  FlashWindowEx(&fi);
#else
  FlashWindow(widget->winId(), TRUE);
#endif
}

#elif defined(Q_WS_MAC)

void Flash::flash(QWidget* /*widget*/) {
}

#else

void Flash::flash(QWidget* /*widget*/) {
}

#endif

