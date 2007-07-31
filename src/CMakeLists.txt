set(tagua_SRC
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
  luaapi/luavalue.cpp

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
  variants/shogi.cpp
  
  hlvariant/chess/variant.cpp
  hlvariant/chess/san.cpp
  hlvariant/chess/move.cpp
  hlvariant/chess/gamestate.cpp
  hlvariant/chess/piece.cpp

  animationfactory.cpp
  constrainedtext.cpp
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
  graphicalgame.cpp
  imageeffects.cpp
  crash.cpp
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
  mastersettings.cpp
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
  option_p.cpp
  themeinfo.cpp
  namedsprite.cpp
  icsgamedata.cpp
  turnpolicy.cpp
)

# remove spurious xpressive warnings
set_source_files_properties(icsconnection.cpp
  PROPERTIES COMPILE_FLAGS -Wno-extra)

kde4_add_ui_files(tagua_SRC
  ui/pref_highlight.ui
  ui/newgamedialog.ui
  ui/pref_engines.ui
  ui/preferences.ui
  ui/gametags.ui
  ui/pref_movelist.ui
  ui/quickconnect.ui
  ui/pref_theme.ui
  ui/pref_theme_page.ui
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

if(NOT DEFINED COMPILER_HAVE_X86_MMX)
  check_cxx_source_compiles(" int main() { __asm__(\"pxor %mm0, %mm0\") ; }" COMPILER_HAVE_X86_MMX)
endif(NOT DEFINED COMPILER_HAVE_X86_MMX)

if(NOT DEFINED COMPILER_HAVE_X86_SSE2)
  check_cxx_source_compiles(" int main() { __asm__(\"xorpd %xmm0, %xmm0\"); }" COMPILER_HAVE_X86_SSE2)
endif(NOT DEFINED COMPILER_HAVE_X86_SSE2)

if(COMPILER_HAVE_X86_MMX)
  list(APPEND tagua_SRC imageeffects_mmx.cpp)
  set_source_files_properties(imageeffects_mmx.cpp PROPERTIES COMPILE_FLAGS -mmmx)
  set_source_files_properties(imageeffects.cpp PROPERTIES COMPILE_FLAGS -DHAVE_X86_MMX)
endif(COMPILER_HAVE_X86_MMX)

if(COMPILER_HAVE_X86_SSE2)
  list(APPEND tagua_SRC imageeffects_sse.cpp)
  set_source_files_properties(imageeffects_sse.cpp PROPERTIES COMPILE_FLAGS -msse2)
  set_source_files_properties(imageeffects.cpp PROPERTIES COMPILE_FLAGS -DHAVE_X86_SSE2)
endif(COMPILER_HAVE_X86_SSE2)

if(DEBUG_BUILD)
  set(TAGUA_TARGET taguaprivate)
  
  kde4_add_library(taguaprivate SHARED ${tagua_SRC})
  kde4_add_executable(tagua main.cpp)
else(DEBUG_BUILD)
  set(TAGUA_TARGET tagua)
  kde4_add_executable(tagua main.cpp ${tagua_SRC})
endif(DEBUG_BUILD)


link_directories(
  ${LUA_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
  ${CMAKE_BINARY_DIR}/lib
)
target_link_libraries(${TAGUA_TARGET}
  ${LUA_LINK_FLAGS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  dl
  kdegames
)  
  
if(DEBUG_BUILD)
  target_link_libraries(tagua taguaprivate)
  install(TARGETS taguaprivate DESTINATION ${LIB_INSTALL_DIR})
endif(DEBUG_BUILD)


INSTALL(TARGETS tagua DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
install(FILES tagua.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES taguaui.rc DESTINATION  ${DATA_INSTALL_DIR}/tagua)
