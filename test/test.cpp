#include "test.h"
#include <iostream>
#include <iomanip>

TEST_CASE("Nothing")
{
  sol::state lua;
  Engine e = useLua(lua);
  
  RUN_LUA(lua, R"(
    print("hi")
  )");

  
}