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
#include <kstandarddirs.h>
#include <kiconloader.h>
#include <klocale.h>

#include "mainwindow.h"
#include "crash.h"
#include "common.h"

static const char description[] =
    I18N_NOOP("A generic board game interface");

static const char version[] = "0.9.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

void trap() {
  printf("Press enter to quit.\n");

  char dummy[4096];
  fgets(dummy, 4096, stdin);
}

int main(int argc, char **argv) {

  KAboutData about("kboard", I18N_NOOP("KBoard"), version, description,
                    KAboutData::License_GPL,
                    "(C) 2006 Paolo Capriotti, Maurizio Monge",
                    0,
                    "http://kboard.sourceforge.net",
                    "p.capriotti@gmail.com");
  about.addAuthor("Paolo Capriotti", 0, "p.capriotti@gmail.com");
  about.addAuthor("Maurizio Monge", 0, "p.capriotti@gmail.com");
  about.addCredit("Jani Huhtanen", I18N_NOOP("Gaussian blur code"), 0, 0);
  about.addCredit("Marcin Jakubowski", I18N_NOOP("X11 taskbar flashing"), 0, 0);
  about.addCredit("Rici Lake", I18N_NOOP("funclib lua library"), 0, 0);

  KCmdLineArgs::init(argc, argv, &about);
  KCmdLineArgs::addCmdLineOptions(options);
  KApplication app;

  installCrashHander();
  atexit(trap);

  QString data_dir = qgetenv("KBOARD_DATA");
  if (data_dir.isEmpty()) data_dir = "data";
  
  KGlobal::dirs()->addResourceDir("appdata", data_dir);
  KGlobal::dirs()->addResourceDir("icon", data_dir + "/pics");
  KGlobal::iconLoader()->reconfigure( app.aboutData()->appName(), KGlobal::dirs() );

//   QString locale = QLocale::system().name();
//
//   QTranslator translator;
//   translator.load(QString("kboard_") + locale);
//   app.installTranslator(&translator);

  MainWindow* widget = new MainWindow;
  widget->show();

  return app.exec();
}

