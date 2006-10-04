/*
  Copyright (c) 2006 Paolo Capriotti <p.capriotti@sns.it>
            (c) 2006 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <map>
#ifdef HAVE_KDE
  #include <kapplication.h>
  #include <kmainwindow.h>
#endif

#include <QApplication>
#include <QMainWindow>
#include <QDir>
#include "boost/shared_ptr.hpp"

#include "ui.h"
#include "common.h"

#ifdef HAVE_KDE
  class KConfig;
#endif
class ChessTable;
class ICSConnection;
class Console;
class AlgebraicNotation;
class Player;
class HistLineEdit;
class Configuration;
class GameInfo;
class PositionInfo;
class Controller;
class QConnect;
class NewGame;
class EngineInfo;
class QDockWidget;
class KTabWidget;
class QStackedWidget;
class QActionGroup;

class MainWindow : public KMainWindow {
Q_OBJECT
  QDockWidget* movelist_dock;
  QDockWidget* console_dock;

  KTabWidget* m_main;
  QStackedWidget* m_movelist_stack;
  boost::shared_ptr<ICSConnection> m_connection;
  Console* console;

  QMenu* m_engine_menu;
  QAction* m_promote_queen;
  QAction* m_promote_rook;
  QAction* m_promote_bishop;
  QAction* m_promote_knight;
  QActionGroup* m_promote_group;

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
  void addPromotionAction(const QString& name, const QString& text, const char* uiSlot);
  void updatePromotionType();

  bool openFile(const QString&);

  void readSettings();
  void writeSettings();
public:
  MainWindow();
  virtual ~MainWindow();

 protected:
   void closeEvent(QCloseEvent*);
   void keyPressEvent(QKeyEvent*);
   void keyReleaseEvent(QKeyEvent*);
private slots:
  void changeTab(int);
  void closeTab();
public slots:
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
  void setupExaminedGame(const GameInfo*, const PositionInfo&);
  void setupObservedGame(const GameInfo*, const PositionInfo&);
  void newGame();
  bool newGame(const QString& var, AbstractPosition::Ptr);
  void loadGame();
  void quit();
  void flipView();
  void toggleConsole();
  void toggleMoveList();
 
  void displayMessage(const QString& msg);
  void displayErrorMessage(ErrorCode);
  void flash();

//  void prefHighlight();
  void prefClock();
  void preferences();
};



#endif // MAINWINDOW_H
