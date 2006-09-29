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

#ifdef Q_OS_UNIX

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

static void handler(int) {
  if(fork())
    while(1){}

  char command[1024];
  const char* args[] = { "/bin/sh", "-c", command, NULL };
  //char *term = "xterm";
  const char* term = "kshell konsole";
  snprintf(command, 1024,
    "%s -e /bin/sh -c "
    "\"echo -e \\\"\\\\33]0;SANGE & CRASHA\\\\7\\\"; "
    "gdb attach %d\"", term, getppid());
  execve(args[0], const_cast<char**>(args), environ);

  kill(getppid(), 9);
}


static void handler2(int) {
  printf("Sange & Crasha!!!!\n");
  printf("I'm sorry, i crashed.\n");
  printf("Now run somewhere\n");
  printf("  gdb attach %d\n", getpid() );
  exit(0);
}

void installCrashHander() {
  signal(SIGSEGV,handler2);
  signal(SIGABRT,handler2);
}

#else

void installCrashHander() {
}

#endif
