
SET(LUA_SRC
  src/lapi.c
  src/lcode.c
  src/ldebug.c
  src/ldo.c
  src/ldump.c
  src/lfunc.c
  src/lgc.c
  src/llex.c
  src/lmem.c
  src/lobject.c
  src/lopcodes.c
  src/lparser.c
  src/lstate.c
  src/lstring.c
  src/ltable.c
  src/ltm.c
  src/lundump.c
  src/lvm.c
  src/lzio.c
  src/lauxlib.c
  src/lbaselib.c
  src/ldblib.c
  src/liolib.c
  src/lmathlib.c
  src/loslib.c
  src/ltablib.c
  src/lstrlib.c
  src/loadlib.c
  src/linit.c
  src/lapi.h
  src/lauxlib.h
  src/lcode.h
  src/ldebug.h
  src/ldo.h
  src/lfunc.h
  src/lgc.h
  src/llex.h
  src/llimits.h
  src/lmem.h
  src/lobject.h
  src/lopcodes.h
  src/lparser.h
  src/lstate.h
  src/lstring.h
  src/ltable.h
  src/ltm.h
  src/luaconf.h
  src/lua.h
  src/lualib.h
  src/lundump.h
  src/lvm.h
  src/lzio.h
)

SET(LUA_HEADERS
  src/lua.h
  src/luaconf.h
  src/lualib.h
  src/lauxlib.h
  etc/lua.hpp
)

FILE(MAKE_DIRECTORY include)
FOREACH(HEADER ${LUA_HEADERS})
  GET_FILENAME_COMPONENT(H ${HEADER} NAME)
  CONFIGURE_FILE(${HEADER} "${LUA_INCLUDE_DIRS}/${H}" COPYONLY)
ENDFOREACH(HEADER)


IF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  ADD_DEFINITIONS(-DLUA_USE_LINUX)
  MESSAGE("* Looks like you're using Linux, lua will be compiled with")
  MESSAGE("    CFlags:    -DLUA_USE_LINUX")
ELSE("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  IF(UNIX)
    MESSAGE("* Looks like you're some Unix, lua will be compiled with")
    MESSAGE("    CFlags:    -DLUA_USE_POSIX -DLUA_USE_DLOPEN")
    MESSAGE("  I hope this is not too optimistic for your system")
    ADD_DEFINITIONS(-DLUA_USE_POSIX -DLUA_USE_DLOPEN)
  ELSE(UNIX)
    MESSAGE("* Could not recognise you system, lua will be compiled with generic options")
  ENDIF(UNIX)
ENDIF("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")

# -DLUA_USE_APICHECK is mandatory for debugging!
set(CMAKE_C_FLAGS_DEBUGFULL "${CMAKE_C_FLAGS_DEBUGFULL} -DLUA_USE_APICHECK")

ADD_LIBRARY(lua STATIC ${LUA_SRC})
