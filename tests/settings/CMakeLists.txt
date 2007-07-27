set(main_dir "../../src")

SET(settings_SRC
  ${main_dir}/mastersettings.cpp
  ${main_dir}/settings.cpp
  settingstest.cpp
  main.cpp
)

include_directories(
  ${KDE4_INCLUDES}
  ${LUA_INCLUDE_DIRS}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/${main_dir}
)

kde4_add_executable(settings_test ${settings_SRC})
target_link_libraries(settings_test ${KDE4_KDEUI_LIBS} ${CPPUNIT_LIBRARIES})

add_test(settings settings_test)
