/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <QtGlobal>
#include "crash.h"

#if defined(Q_OS_UNIX) && defined(TAGUA_DEBUG)

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static void handler(int) {
  printf("Sange & Crasha!!!!\n");
  printf("I'm sorry, i crashed.\n");
  printf("Now run somewhere\n");
  printf("  gdb attach %d\n", getpid() );
  exit(0);
}
 

void CrashHandler::install() {
  signal(SIGSEGV, handler);
  signal(SIGABRT, handler);
}

#else

void CrashHandler::install() { }

#endif
