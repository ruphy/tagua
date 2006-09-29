
SET(this_test luaimage)


FILE(GLOB_RECURSE ${this_test}_SRC_CXX *.cpp)
FILE(GLOB_RECURSE ${this_test}_SRC_C *.c)
SET(${this_test}_SRC ${${this_test}_SRC_CXX} ${${this_test}_SRC_C})

FILE(GLOB_RECURSE ${this_test}_UIS *.ui)


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
  .
  ${QT_INCLUDE_DIR}
  #${QT_QTNETWORK_INCLUDE_DIR}
  ${QT_QTSVG_INCLUDE_DIR}
  ${LUA_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${KDE4_INCLUDE_DIR}
)

ADD_DEFINITIONS(
  ${LUA_CFLAGS}
)

LINK_DIRECTORIES(
  ${LUA_LIBRARY_DIRS}
  ${Boost_LIBRARY_DIRS}
)

TARGET_LINK_LIBRARIES(${this_test}
  ${QT_LIBRARIES}
  #${QT_QTNETWORK_LIBRARIES}
  ${QT_QTSVG_LIBRARIES}
  ${LUA_LINK_FLAGS}
  ${KDE4_KDEUI_LIBS}
  ${KDE4_KIO_LIBS}
)

