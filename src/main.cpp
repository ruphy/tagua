/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kglobal.h>
#include <klocale.h>
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>

#include "mainwindow.h"
#include "crash.h"
#include "common.h"

static const char description[] = "A generic board game interface";

static const char version[] = "0.9.1";

void trap() {
  printf("Press enter to quit.\n");

  char dummy[4096];
  fgets(dummy, 4096, stdin);
}

int main(int argc, char **argv) {
  KAboutData about( "tagua", 0, ki18n("Tagua"),
    version, ki18n(description), KAboutData::License_GPL,
    ki18n("(C) 2006 Paolo Capriotti, Maurizio Monge") );
  about.addAuthor(ki18n("Paolo Capriotti"), KLocalizedString(), "p.capriotti@gmail.com");
  about.addAuthor(ki18n("Maurizio Monge"), KLocalizedString(), "p.capriotti@gmail.com");
  about.addCredit(ki18n("Jani Huhtanen"), KLocalizedString(), I18N_NOOP("Gaussian blur code") );
  about.addCredit(ki18n("Rici Lake"), KLocalizedString(), I18N_NOOP("funclib lua library") );

  KCmdLineOptions options;
  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KApplication app;

#ifdef TAGUA_DEBUG
  // So you can attach the debugger in case of segfault.
  installCrashHander();
  atexit(trap);

  // If we are debugging, it is handy to have a local data
  // directory which is accessed by Tagua, so we don't need
  // to reconfigure / reinstall whenever a change is made in
  // a theme.
  QString data_dir = qgetenv("TAGUA_DATA");
  if (data_dir.isEmpty()) data_dir = "data";

  KGlobal::dirs()->addResourceDir("appdata", data_dir, true);
  KGlobal::dirs()->addResourceDir("icon", data_dir + "/pics", true);
  KIconLoader::global()->reconfigure(about.appName(), KGlobal::dirs());
#endif // KBOARD_DEBUG

  MainWindow* widget = new MainWindow;
  widget->show();

  return app.exec();
}

