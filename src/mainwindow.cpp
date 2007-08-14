/*
  Copyright (c) 2006-2007 Paolo Capriotti <p.capriotti@gmail.com>
            (c) 2006-2007 Maurizio Monge <maurizio.monge@kdemail.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
*/

#include "mainwindow.h"
#include <boost/scoped_ptr.hpp>
#include <kaction.h>
#include <kstandardaction.h>
#include <kactioncollection.h>
#include <kicon.h>
#include <klocale.h>
#include <kurl.h>
#include <kfiledialog.h>
#include <kio/netaccess.h>
#include <kmessagebox.h>
#include <kmenubar.h>
#include <QKeySequence>
#include <QStackedWidget>
#include <QDockWidget>
#include <QCloseEvent>
#include <QTextStream>
#include <QTextCodec>
#include <ktabwidget.h>

#include "actioncollection.h"
#include "chesstable.h"
#include "console.h"
#include "clock.h"
#include "newgame.h"
#include "variants.h"
#include "gameinfo.h"
#include "controllers/editgame.h"
#include "controllers/editposition.h"
#include "engineinfo.h"
#include "movelist_table.h"
#include "icsconnection.h"
#include "qconnect.h"
#include "mastersettings.h"
#include "flash.h"
#include "foreach.h"
#include "pgnparser.h"
#include "pref_highlight.h"
#include "pref_preferences.h"

using namespace Qt;
using namespace boost;

MainWindow::~MainWindow() {
  delete console;
  qApp->quit();
}

MainWindow::MainWindow(const QString& variant)
: KXmlGuiWindow(0) {
  setObjectName("tagua_main");
  m_main = new KTabWidget(this);
  setCentralWidget(m_main);

  m_movelist_stack = new QStackedWidget;

  connect(m_main, SIGNAL(currentChanged(int)),
    this, SLOT(changeTab(int)));
  connect(m_main, SIGNAL(closeTab()),
    this, SLOT(closeTab()));

  movelist_dock = new QDockWidget(this);
  movelist_dock->setWidget(m_movelist_stack);
  movelist_dock->setWindowTitle(i18n("Move list"));
  movelist_dock->setObjectName("move_list");
  addDockWidget(Qt::LeftDockWidgetArea, movelist_dock, Qt::Vertical);
  movelist_dock->show();

  ChessTable* board = new ChessTable;

  board->setFocus();

  console_dock = new QDockWidget(this);
  console = new Console(0, i18n("FICS Connection"));
  console_dock->setWidget(console);
  console_dock->setFocusProxy(console);
  console_dock->setWindowTitle(i18n("Console"));
  console_dock->setObjectName("console");
  addDockWidget(Qt::BottomDockWidgetArea, console_dock, Qt::Horizontal);
  console_dock->setWindowFlags(console_dock->windowFlags() & ~Qt::WindowStaysOnTopHint);
  console_dock->show();

  connect(board, SIGNAL(error(ErrorCode)), this, SLOT(displayErrorMessage(ErrorCode)));
  //BROKEN connect(board->clock(), SIGNAL(labelClicked(int)), &ui(), SLOT(setTurn(int)));

//  start in edit game mode
  newGame(variant, AbstractPosition::Ptr());

  setupActions();
  setupGUI();
  setupEngineMenu();
  updateVariantActions();
}

ChessTable* MainWindow::table() {
  return qobject_cast<ChessTable*>(m_main->currentWidget());
}

KAction* MainWindow::installRegularAction(const QString& name, const KIcon& icon, const QString& text, QObject* obj, const char* slot) {
  KAction* temp = new KAction(icon, text, this);
  actionCollection()->addAction(name, temp);
  connect(temp, SIGNAL(triggered(bool)), obj, slot);
  return temp;
}

void MainWindow::setupEngineMenu() {
  QMenu* engine_menu = 0;
  SettingArray engine_settings = settings().group("engines").array("engine");
  foreach (Settings s, engine_settings) {
    if (!engine_menu) {
      // this way the menu is created only if there is at least one engine
      engine_menu = menuBar()->addMenu(i18n("E&ngines"));
    }

    QString name;
    EngineDetails engine_details;
    s["name"] >> engine_details.name;
    s["path"] >> engine_details.path;
    engine_details.type = EngineDetails::typeFromName(s["type"].value<QString>());
    if (s["work-path"])
      s["work-path"] >> engine_details.workPath;

    kDebug() << "creating engine " << engine_details.name << endl;
    EngineInfo* engine = new EngineInfo(engine_details, ui());

    m_engines.push_back(engine);

    QMenu* menu = engine_menu->addMenu(engine_details.name);

    {
      KAction* play_white = new KAction(i18n("Play as &white"), this);
      play_white->setCheckable(true);
      connect(play_white, SIGNAL(triggered()), engine, SLOT(playAsWhite()));
      menu->addAction(play_white);
    }
    {
      KAction* play_black = new KAction(i18n("Play as &black"), this);
      play_black->setCheckable(true);
      connect(play_black, SIGNAL(triggered()), engine, SLOT(playAsBlack()));
      menu->addAction(play_black);
    }
#if 0 // disable analysis for the moment
    {
      KAction* analyze = new KAction(i18n("&Analyze"), this);
      analyze->setCheckable(true);
      connect(analyze, SIGNAL(triggered()), engine, SLOT(analyze()));
      menu->addAction(analyze);
    }
#endif
  }
}

void MainWindow::setupActions() {
  KStandardAction::openNew(this, SLOT(newGame()), actionCollection());
  KStandardAction::open(this, SLOT(loadGame()), actionCollection());
  KStandardAction::quit(this, SLOT(quit()), actionCollection());

  installRegularAction("back", KIcon("go-previous"), i18n("&Back"), &ui(), SLOT(back()));
  installRegularAction("forward", KIcon("go-next"), i18n("&Forward"), &ui(), SLOT(forward()));
  installRegularAction("begin", KIcon("go-first"), i18n("Be&gin"), &ui(), SLOT(gotoFirst()));
  installRegularAction("end", KIcon("go-last"), i18n("&End"), &ui(), SLOT(gotoLast()));
  installRegularAction("connect", KIcon("connection-established"), i18n("&Connect"), this, SLOT(icsConnect()));
  installRegularAction("disconnect", KIcon("connect-no"), i18n("&Disconnect"), this, SLOT(icsDisconnect()));

  KStandardAction::undo(&ui(), SLOT(undo()), actionCollection());
  KStandardAction::redo(&ui(), SLOT(redo()), actionCollection());

  installRegularAction("pgnCopy", KIcon("edit-copy"), i18n("Copy PGN"), this, SLOT(pgnCopy()));
  installRegularAction("pgnPaste", KIcon("edit-paste"), i18n("Paste PGN"), this, SLOT(pgnPaste()));
  installRegularAction("editPosition", KIcon("edit"), i18n("&Edit position"), this, SLOT(editPosition()));
  installRegularAction("clearBoard", KIcon("edit-delete"), i18n("&Clear board"), &ui(), SLOT(clearBoard()));
  installRegularAction("setStartingPosition", KIcon("contents"), i18n("&Set starting position"),
      &ui(), SLOT(setStartingPosition()));
  installRegularAction("copyPosition", KIcon(), i18n("&Copy position"), &ui(), SLOT(copyPosition()));
  installRegularAction("pastePosition", KIcon(), i18n("&Paste position"), &ui(), SLOT(pastePosition()));
  installRegularAction("flip", KIcon("rotate"), i18n("&Flip view"), this, SLOT(flipView()));
  installRegularAction("toggleConsole", KIcon("openterm"), i18n("Toggle &console"), this, SLOT(toggleConsole()));
  installRegularAction("toggleMoveList", KIcon("view_text"), i18n("Toggle &move list"), this, SLOT(toggleMoveList()));
  installRegularAction("configure", KIcon("configure"), i18n("&Configure Tagua..."), this, SLOT(preferences()));
}

void MainWindow::updateVariantActions() {
  ActionCollection* variant_actions = m_ui.variantActions();
  unplugActionList("variantActions");
  if (variant_actions) {
    plugActionList("variantActions", variant_actions->actions());
  }
  else {
    WARNING("No variant actions");
  }
}

void MainWindow::readSettings() { }
void MainWindow::writeSettings() { }

void MainWindow::closeEvent(QCloseEvent* e) {
  e->accept();
  writeSettings();
  delete this;
}


void MainWindow::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_F5) {
    ui().createCtrlAction();
  }
}

void MainWindow::keyReleaseEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_F5) {
    ui().destroyCtrlAction();
  }
}

void MainWindow::changeTab(int index) {
  m_ui.setCurrentTab(m_main->currentWidget());
  m_movelist_stack->setCurrentIndex(index);
  updateVariantActions();
}

void MainWindow::closeTab() {
  if (m_main->count() > 1) {
    ChessTable* old_board = table();
    m_ui.removeController(old_board);
    m_movelist_stack->removeWidget(
      m_movelist_stack->widget(m_main->currentIndex()));
    m_main->removeTab(m_main->currentIndex());

    delete old_board;

    if (m_main->count() <= 1) {
      m_main->setTabBarHidden(true);
    }

    // update ui controller (just in case...)
    m_ui.setCurrentTab(m_main->currentWidget());
  }
}

void MainWindow::createTab(ChessTable* board, const shared_ptr<Controller>& controller,
                           const QString& caption, int index) {
  if (index == -1)
    index = m_main->addTab(board, caption);
  else
    m_main->insertTab(index, board, caption);

  m_ui.addController(board, controller);
  m_ui.setCurrentTab(board);
  m_movelist_stack->addWidget(board->moveListTable());
  
  m_main->setCurrentIndex(index);
  m_movelist_stack->setCurrentIndex(index);
  
  if (m_main->count() > 1) m_main->setTabBarHidden(false);
}


void MainWindow::cleanupGame(const QString& what, const QString& result) {
  Q_UNUSED(what);
  Q_UNUSED(result);

  cleanupGame();
}

void MainWindow::cleanupGame() {
  ui().detach();
  ui().end();
}


bool MainWindow::newGame(const QString& variantName, AbstractPosition::Ptr startingPos) {
  VariantPtr variant = Variants::instance().get(variantName);
  if (!variant) {
    WARNING("no variant " << variantName << " found");
    variant = Variants::instance().get("chess");
  }

  if (variant) {
    ChessTable* board = new ChessTable;
    shared_ptr<Controller> controller(new EditGameController(
      board, variant, startingPos));
    createTab(board, controller,
      QString("Editing %1 game").arg(variant->name().toLower()));
    return true;
  }
  else {
    ERROR("Could not find the chess variant");
    exit(1);
    return false;
  }
}


void MainWindow::editPosition() {
//BROKEN
#if 0
  shared_ptr<Controller> controller(new EditPositionController(table(), ChessVariant::info()));
  m_main->setTabText(m_main->currentIndex(), "Editing chess position");
  ui().setController(controller);
#endif
}

void MainWindow::setupGame(const GameInfo* gameInfo, const PositionInfo& style12) {
  QString variantCode = gameInfo->variant();
  VariantPtr variant = Variants::instance().get(variantCode);
  shared_ptr<EditGameController> controller(new EditGameController(
    table(), variant));
  Q_ASSERT(style12.relation == PositionInfo::NotMyMove ||
           style12.relation == PositionInfo::MyMove);

  // set opponent side
  int side = (style12.relation == PositionInfo::MyMove ^ style12.turn == 0) ? 0 : 1;

  if (controller->addICSPlayer(side, style12.gameNumber, m_connection)) {
    ui().setController(controller);
    table()->setPlayers(gameInfo->white(), gameInfo->black());
    m_main->setTabText(m_main->currentIndex(),
      QString("FICS Game - %1 vs %2").arg(style12.whitePlayer)
                                     .arg(style12.blackPlayer));
  }
}

void MainWindow::setupExaminedGame(const GameInfo* /*gameInfo*/, const PositionInfo& style12) {
  shared_ptr<EditGameController> controller(new EditGameController(
                                      table(), Variants::instance().get("chess")));
  if (controller->setExaminationMode(style12.gameNumber, m_connection)) {
    table()->setPlayers(Player(style12.whitePlayer, -1),
                      Player(style12.blackPlayer, -1));
    ui().setController(controller);
    m_main->setTabText(m_main->currentIndex(),
      QString("Examining - %1 vs %2").arg(style12.whitePlayer)
                                     .arg(style12.blackPlayer));
  }

}

void MainWindow::setupObservedGame(const GameInfo* g, const PositionInfo& style12) {
  std::auto_ptr<ChessTable> board(new ChessTable);

  shared_ptr<EditGameController> controller(new EditGameController(
                                      board.get(), Variants::instance().get(g->variant())));
  if (controller->setObserveMode(style12.gameNumber, m_connection)) {
    board->setPlayers(Player(style12.whitePlayer, -1),
                      Player(style12.blackPlayer, -1));
//    ui().setController(controller);
    //std::cout << "adding tab" << std::endl;
    createTab(board.get(), controller,
      QString("Observing - %1 vs %2").arg(style12.whitePlayer)
                                     .arg(style12.blackPlayer));
    board.release();
  }
}

void MainWindow::setupPGN(const QString& s) {
  PGN pgn(s);

  std::map<QString, QString>::const_iterator var = pgn.m_tags.find("Variant");
  VariantPtr variant;

  if (var == pgn.m_tags.end()) {
    variant = Variants::instance().get("chess");
  }
  else if (!(variant = Variants::instance().get(var->second))) {
    std::cout << " --> MainWindow::setupPGN: Error, no such variant " << var->second << std::endl;
    return;
  }

  shared_ptr<EditGameController> controller(new EditGameController(
                                              table(), variant));
  ui().setController(controller);
  controller->loadPGN(pgn);

//   table()->setPlayers(gameInfo->white(), gameInfo->black());
//   m_main->setTabText(m_main->currentIndex(),
//     QString("FICS Game - %1 vs %2").arg(style12.whitePlayer)
//                                     .arg(style12.blackPlayer));
}

bool MainWindow::openFile(const QString& filename) {
  QFileInfo info(filename);

  if(info.isDir()) {
     KMessageBox::sorry(this, i18n("You have specified a folder"), i18n("Error"));
     return false;
  }

  if(!info.exists() || !info.isFile()) {
     KMessageBox::sorry(this, i18n("The specified file does not exist"), i18n("Error"));
     return false;
  }

  QFile file(filename);

  if(!file.open(QIODevice::ReadOnly)) {
     KMessageBox::sorry(this, i18n("You do not have read permission to this file."), i18n("Error"));
     return false;
  }

  QTextStream stream(&file);
  QTextCodec *codec;
  codec = QTextCodec::codecForLocale();
  stream.setCodec(codec);

  setupPGN(stream.readAll());
  //ui().pgnPaste(stream.readAll());
  return true;
}

void MainWindow::loadGame() {
  KUrl url = KFileDialog::getOpenUrl(KUrl(), "*.pgn", this, i18n("Open PGN file"));

  if(url.isEmpty())
    return;

  QString tmp_file;
  if (KIO::NetAccess::download(url, tmp_file, this)) {
    openFile(tmp_file);
    KIO::NetAccess::removeTempFile(tmp_file);
  }
  else
    KMessageBox::error(this, KIO::NetAccess::lastErrorString());
}

void MainWindow::createConnection(const QString& username, const QString& password,
                                  const QString& host, quint16 port,
                                  const QString& timeseal, const QString& timeseal_cmd) {
  m_connection = shared_ptr<ICSConnection>(new ICSConnection);

  connect(m_connection.get(), SIGNAL(established()), this, SLOT(onEstablishConnection()));
  connect(m_connection.get(), SIGNAL(hostLookup()), this, SLOT(onHostLookup()));
  connect(m_connection.get(), SIGNAL(hostFound()), this, SLOT(onHostFound()));

  connect(m_connection.get(), SIGNAL(receivedLine(QString, int)), console, SLOT(displayText(QString, int)));
  connect(m_connection.get(), SIGNAL(receivedText(QString, int)), console, SLOT(displayText(QString, int)));

  connect(console, SIGNAL(receivedInput(const QString&)), m_connection.get(), SLOT(sendText(const QString&)));
  connect(console, SIGNAL(notify()), this, SLOT(flash()));

  connect(m_connection.get(), SIGNAL(loginPrompt()), this, SLOT(sendLogin()));
  connect(m_connection.get(), SIGNAL(registeredNickname()), this, SLOT(sendBlankPassword()));
  connect(m_connection.get(), SIGNAL(prompt()), this, SLOT(prompt()));


  connect(m_connection.get(), SIGNAL(creatingExaminedGame(const GameInfo*, const PositionInfo&)),
          this, SLOT(setupExaminedGame(const GameInfo*, const PositionInfo&)));
  connect(m_connection.get(), SIGNAL(endingExaminedGame(int)), this, SLOT(cleanupGame()));

  connect(m_connection.get(), SIGNAL(creatingObservedGame(const GameInfo*, const PositionInfo&)),
          this, SLOT(setupObservedGame(const GameInfo*, const PositionInfo&)));
  connect(m_connection.get(), SIGNAL(endingObservedGame(int)), this, SLOT(cleanupGame()));


  connect(m_connection.get(), SIGNAL(creatingGame(const GameInfo*, const PositionInfo&)),
          this, SLOT(setupGame(const GameInfo*, const PositionInfo&)));
  connect(m_connection.get(), SIGNAL(endingGame(const QString&, const QString&)),
          this, SLOT(cleanupGame(const QString&, const QString&)));
  connect(m_connection.get(), SIGNAL(notification()), this, SLOT(flash()));

  m_connection->establish(host.toAscii().constData(), port, timeseal, timeseal_cmd);
  console->show();

  // send username / password combination
  if (!username.isEmpty()) {
    m_connection->sendText(username);
    m_connection->sendText(password);
  }

  quickConnectDialog.reset();
}

void MainWindow::icsConnect() {
  quickConnectDialog = shared_ptr<QConnect>(new QConnect(this));
  connect(quickConnectDialog.get(),
          SIGNAL(acceptConnection(const QString&,
                                  const QString&,
                                  const QString&,
                                  quint16,
                                  const QString&,
                                  const QString&)),
          this,
          SLOT(createConnection(const QString&,
                                const QString&,
                                const QString&,
                                quint16,
                                const QString&,
                                const QString&)));
  quickConnectDialog->show();
}

void MainWindow::destroyConnection() {
  m_connection.reset();
}

void MainWindow::icsDisconnect() {
  destroyConnection();
  console->hide();
  console->clear();
}

void MainWindow::testConnect() {
  Settings s_ics = settings().group("ics");
  if (s_ics["username"]) {
    QString username = s_ics["username"].value<QString>();
    QString password = (s_ics["password"] | "");
    QString host = (s_ics["icsHost"] | "freechess.org");
    quint16 port = (s_ics["icsPort"] | 5000);
    createConnection(username, password, host, port, QString(), QString() );
  }
  else icsConnect();
}


void MainWindow::onEstablishConnection() {
//  std::cout << "connection established" << std::endl;
}

void MainWindow::onConnectionError(int ) {
//  std::cout << "connection error (" << code << ")" << std::endl;
}

void MainWindow::onHostLookup() {
//  std::cout << "hostLookup..." << std::flush;
}

void MainWindow::onHostFound() {
//  std::cout << "found" << std::endl;
}


void MainWindow::sendLogin() {
//  std::cout << "sending username" << std::endl;
//  connection->sendText(connection->username());
}

void MainWindow::sendBlankPassword() {
  std::cout << "sending blank password" << std::endl;
  m_connection->sendText("");
}

void MainWindow::prompt() {
  if (!m_connection->initialized()) {
    m_connection->startup();
    m_connection->setInitialized();
  }
}

void MainWindow::newGame() {
  AbstractPosition::Ptr pos = ui().position();
  scoped_ptr<NewGame> dialog(new NewGame(this, pos));
  if (dialog->exec() == QDialog::Accepted) {
    if(dialog->isCustom()) {
      VariantPtr vi = Variants::instance().get(dialog->variant());
      std::cout << "vi[" << dialog->variant() << "] = " << vi << std::endl;
      pos = vi->createCustomPosition(dialog->customOptions());
      pos->setup();
    }
    else if (!dialog->playFromCurrent())
      pos = AbstractPosition::Ptr();
    if (!newGame(dialog->variant(), pos))
      QMessageBox::information(this, tr("Error"), tr("Variant not implemented, yet"));
  }
}

void MainWindow::quit() {
  delete this;
}

void MainWindow::flipView() {
  table()->flip();
}

void MainWindow::toggleConsole() {
  if (console_dock->isVisible())
    console_dock->hide();
  else {
    console_dock->show();
    console_dock->setFocus(Qt::MouseFocusReason
          /*Qt::ActiveWindowFocusReason*/ /*Qt::OtherFocusReason*/);
  }
}

void MainWindow::toggleMoveList() {
  if (movelist_dock->isVisible())
    movelist_dock->hide();
  else {
    movelist_dock->show();
    movelist_dock->setFocus(Qt::OtherFocusReason);
  }
}


void MainWindow::displayMessage(const QString& msg) {
  Q_UNUSED(msg); // TODO
//   statusBar()->message(msg, 2000);
}

void MainWindow::displayErrorMessage(ErrorCode code) {
  switch(code) {
  case InvalidMove:
    displayMessage(tr("Illegal move"));
    break;
  }
}

void MainWindow::flash() {
  if( !isAncestorOf(QApplication::focusWidget()) )
    Flash::flash(this);
}

#if 0
void MainWindow::prefHighlight() {
  PrefHighlight dialog;
  int result = dialog.exec();
  if (result == QDialog::Accepted) {
    dialog.apply();
  }
}
#endif

void MainWindow::preferences() {
  Preferences dialog(ui().currentVariant());
  int result = dialog.exec();
  if (result == QDialog::Accepted)
    dialog.apply();
}



