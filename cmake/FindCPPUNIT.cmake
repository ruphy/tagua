# - Try to find the libcppunit libraries
# Once done this will define
#
# CPPUNIT_FOUND - system has libcppunit
# CPPUNIT_INCLUDE_DIR - the libcppunit include directory
# CPPUNIT_LIBRARIES - libcppunit library

FIND_PATH(CPPUNIT_INCLUDE_DIR Test.h PATHS /usr/include/cppunit /usr/local/include/cppunit )
FIND_LIBRARY(CPPUNIT_LIBRARIES NAMES cppunit PATHS /usr/lib /usr/local/lib )

# IF(CPPUNIT_INCLUDE_DIR)
#     message (STATUS "found cppunit include dir: ${CPPUNIT_INCLUDE_DIR}")
# ENDIF (CPPUNIT_INCLUDE_DIR)
# 
# IF(CPPUNIT_LIBRARIES)
#     message (STATUS "found cppunit libraries: ${CPPUNIT_LIBRARIES}")
# ENDIF (CPPUNIT_LIBRARIES)

IF(CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)
  SET(CPPUNIT_FOUND 1   )
  if(NOT CPPUNIT_FIND_QUIETLY)
   message(STATUS "Found cppunit: ${CPPUNIT_LIBRARIES}")
  endif(NOT CPPUNIT_FIND_QUIETLY)
ELSE(CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)
  SET(CPPUNIT_FOUND 0 CACHE BOOL "Not found cppunit library")
ENDIF(CPPUNIT_INCLUDE_DIR AND CPPUNIT_LIBRARIES)

MARK_AS_ADVANCED(CPPUNIT_INCLUDE_DIR CPPUNIT_LIBRARIES)
