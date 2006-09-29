#include "../src/luaconfig.h"

using namespace LuaLoader;

int main(int argc, char** argv) {
  const char* file;
  if (argc > 1)
    file = argv[1];
  else
    file = "test.lua";

  Api api;
  api.runFile(file);
  
  return 0;
}

