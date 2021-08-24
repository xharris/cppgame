#include "raylib.hpp"
#include "engine.h"

int main(int argc, char** argv)
{
  // LuaScript script = LuaScript("main.lua");
  // if (script.call("load", 1, { new LuaNumber<int>(5) }))
  //   exit(0);

  // setup_raylib();

  Engine engine = Engine();
  engine.go();

  return 0;
}
