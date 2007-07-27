# some tests require cppunit
find_package(CPPUNIT)
if(CPPUNIT_FOUND)
  add_subdirectory(settings)
else(CPPUNIT_FOUND)
  message("CppUnit not found. Tests requiring it will not be compiled.")
endif(CPPUNIT_FOUND)

# ADD_SUBDIRECTORY(luaimage)
# ADD_SUBDIRECTORY(options)
# ADD_SUBDIRECTORY(movelist)
