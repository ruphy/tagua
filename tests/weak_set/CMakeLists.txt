set(main_dir "../../src")

SET(weakset_SRC
  weakset_test.cpp
  ../cppunit_main.cpp
)

include_directories(
  ${Boost_INCLUDE_DIRS}
  ${CMAKE_CURRENT_BINARY_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/${main_dir}
)

add_executable(weakset_test ${weakset_SRC})
target_link_libraries(weakset_test ${CPPUNIT_LIBRARIES})

add_test(weakset weakset_test)
