set(kboard_SRC
  controllers/editgame.cpp
  controllers/abstract.cpp
  controllers/editposition.cpp

  loader/image.cpp
  loader/theme.cpp
  loader/context.cpp

  luaapi/lfunclib.c
  luaapi/options.cpp
  luaapi/luahl.cpp
  luaapi/genericwrapper.cpp
  luaapi/loader.cpp
  luaapi/imaging.cpp

  entities/gameentity.cpp
  entities/userentity.cpp
  entities/examinationentity.cpp
  entities/entity.cpp
  entities/engineentity.cpp
  entities/icsentity.cpp

  variants/variants.cpp
  variants/xchess/move.cpp
  variants/xchess/piece.cpp
  variants/chess.cpp
  variants/crazyhouse.cpp
  
  animationfactory.cpp
  movelist.cpp
  infodisplay.cpp
  engineinfo.cpp
  premove.cpp
  mainanimation.cpp
  random.cpp
  point.cpp
  sprite.cpp
  pref_movelist.cpp
  option.cpp
  graphicalsystem.cpp
  agentgroup.cpp
  main.cpp
  graphicalgame.cpp
  imageeffects.cpp
  crash.cpp
  pref_clock.cpp
  flash.cpp
  histlineedit.cpp
  pathinfo.cpp
  pref_theme.cpp
  gameinfo.cpp
  console.cpp
  animation.cpp
  pref_engines.cpp
  clock.cpp
  chesstable.cpp
  index.cpp
  algebraicnotation.cpp
  global.cpp
  location.cpp
  hline.cpp
  xboardengine.cpp
  settings.cpp
  positioninfo.cpp
  engine.cpp
  ui.cpp
  movelist_widget.cpp
  pref_preferences.cpp
  poolinfo.cpp
  pixmaploader.cpp
  qconnect.cpp
  pref_board.cpp
  game.cpp
  piecepool.cpp
  piecegroup.cpp
  movelist_textual.cpp
  icsconnection.cpp
  mainwindow.cpp
  board.cpp
  common.cpp
  pgnparser.cpp
  movement.cpp
  connection.cpp
  movelist_table.cpp
  newgame.cpp
)

kde4_add_ui_files(kboard_SRC
  ui/pref_highlight.ui
  ui/newgamedialog.ui
  ui/pref_engines.ui
  ui/preferences.ui
  ui/gametags.ui
  ui/pref_movelist.ui
  ui/quickconnect.ui
  ui/pref_theme.ui
  ui/pref_board.ui
)

include_directories(
  ${KDE4_INCLUDES}
  ${LUA_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
)

add_definitions(
  ${LUA_CFLAGS}
)


kde4_add_executable(kboard ${kboard_SRC})

link_directories(
  ${LUA_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}

)

message("lua: " ${LUA_LINK_FLAGS})

target_link_libraries(kboard
  ${LUA_LINK_FLAGS}
  ${KDE4_KDEUI_LIBS}
#   ${KDE4_KIO_LIBS}
#   ${KDE4_KHTML_LIBS}
  kdegames
)



INSTALL(TARGETS kboard
  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
