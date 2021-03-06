/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#include <vector>
#include <KXmlGuiWindow>

#include <QApplication>
#include <QMainWindow>
#include <QDir>
#include "boost/shared_ptr.hpp"

#include "ui.h"
#include "common.h"
#include "export.h"

class ChessTable;
class ICSConnection;
class Console;
class GameInfo;
class PositionInfo;
class Controller;
class QConnect;
class NewGame;
class EngineInfo;
class QDockWidget;
class TabWidget;
class QStackedWidget;
class QActionGroup;
class KAction;
class KIcon;

class TAGUA_EXPORT MainWindow : public KXmlGuiWindow {
Q_OBJECT
  QDockWidget* movelist_dock;
  QDockWidget* console_dock;

  TabWidget* m_main;
  QStackedWidget* m_movelist_stack;
  boost::shared_ptr<ICSConnection> m_connection;
  Console* console;

  QMenu* m_engine_menu;
  
  ChessTable* table();
  UI m_ui;
  UI& ui() { return m_ui; }
  void createTab(ChessTable*, const boost::shared_ptr<Controller>&,
                 const QString& label, int index = -1);
//
  QDir m_iconDir;
  std::vector<EngineInfo*> m_engines;

  boost::shared_ptr<QConnect> quickConnectDialog;
  NewGame* newGameDialog;
//
//   QAction *mkAction(const QString& txt, QKeySequence shk, QObject *o,
//                     const char *sl, QString name, QObject *par = NULL);
  void setupActions();
  void setupEngineMenu();
  KAction* addPromotionAction(const QString& name, const QString& text, const char* uiSlot);
  KAction* installRegularAction(const QString& name, const KIcon& icon, const QString& text,
                                QObject* obj, const char* slot);
  void updateVariantActions(bool unplug = true);

  bool openFile(const QString&);
  void saveFile(QFile&);
  KUrl saveGame(const KUrl& url);

  void readSettings();
  void writeSettings();
public:
  MainWindow(const QString& variant);
  virtual ~MainWindow();

 protected:
   void closeEvent(QCloseEvent*);
   void keyPressEvent(QKeyEvent*);
   void keyReleaseEvent(QKeyEvent*);
private Q_SLOTS:
  void changeTab(int);
  void closeTab();
public Q_SLOTS:
  void createConnection(const QString&, const QString&, const QString&,
                                quint16, const QString&, const QString&);
  void destroyConnection();
  void testConnect(); // DEBUG
  void icsConnect();
  void icsDisconnect();
// //  void receivedCommand(const QString&);
//
  void onEstablishConnection();
  void onHostLookup();
  void onConnectionError(int);
  void onHostFound();
//
  void sendLogin();
  void sendBlankPassword();
  void prompt();
  void cleanupGame(const QString&, const QString&);
  void cleanupGame();

  void editPosition();
  void setupGame(const GameInfo*, const PositionInfo&);
  void setupPGN(const QString& s);
  void setupExaminedGame(const GameInfo*, const PositionInfo&);
  void setupObservedGame(const GameInfo*, const PositionInfo&);
  void newGame();
  bool newGame(const QString& var, AbstractPosition::Ptr, bool);
  void loadGame();
  void saveGame();
  void saveGameAs();
  bool checkOverwrite(const KUrl& url);
  void quit();
  void flipView();
  void toggleConsole();
  void toggleMoveList();

  void displayMessage(const QString& msg);
  void displayErrorMessage(ErrorCode);
  void flash();

//  void prefHighlight();
  void preferences();
  void settingsChanged();
};



#endif // MAINWINDOW_H
