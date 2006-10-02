
SET(this_test movelist)
SETUP_SOURCES(${this_test}_SRC ${this_test}_UIS)


# i have to do this because QT4_WRAP_UI is stupid
SET(old_bin_dir ${CMAKE_CURRENT_BINARY_DIR})
SET(CMAKE_CURRENT_BINARY_DIR ${old_bin_dir}/ui)
QT4_WRAP_UI(${this_test}_UIS_H ${${this_test}_UIS})
SET(CMAKE_CURRENT_BINARY_DIR ${old_bin_dir})


# QT4_AUTOMOC is not very intelligent either
QT4_AUTOMOC(
  ${${this_test}_SRC})

ADD_EXECUTABLE(
  ${this_test} ${${this_test}_SRC})


INCLUDE_DIRECTORIES(
  ${QT_INCLUDE_DIR}
  #${QT_QTNETWORK_INCLUDE_DIR}
  #${QT_QTSVG_INCLUDE_DIR}
  ${QT_QTXML_INCLUDE_DIR}
  #${LUA_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${KDE4_INCLUDE_DIR}
  .
  ${CMAKE_CURRENT_BINARY_DIR}
)

ADD_DEFINITIONS(
  ${LUA_CFLAGS}
  -DNO_PGN
)

LINK_DIRECTORIES(
  #${LUA_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
)

TARGET_LINK_LIBRARIES(${this_test}
  ${QT_LIBRARIES}
  #${QT_QTNETWORK_LIBRARIES}
  #${QT_QTSVG_LIBRARIES}
  ${QT_QTXML_LIBRARIES}
  #${LUA_LINK_FLAGS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
  ${KDE4_KHTML_LIBS}
)

