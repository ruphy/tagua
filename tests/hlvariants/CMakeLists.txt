set(main_dir "../../src")

SET(hl_SRC
  ../cppunit_main.cpp
  boardtest.cpp
  chesspiecetest.cpp
  chessgamestatetest.cpp
  chessmovetest.cpp
  chesslegalitytest.cpp
  chesswrappedtest.cpp
  chessserializationtest.cpp
)

include_directories(
  ${KDE4_INCLUDES}
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/${main_dir}
)

add_executable(hl_test ${hl_SRC})
target_link_libraries(hl_test taguaprivate ${CPPUNIT_LIBRARIES})

add_test(hl hl_test)
