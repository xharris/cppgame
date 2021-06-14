#include "test.h"
#include <iostream>
#include <iomanip>

TEST_CASE("ECS & Scene Graph")
{
  sol::state lua;
  Engine e = useLua(lua);
  
  RUN_LUA(lua, R"(
    solar_system    = node()
    sun             = node{x=game.width/2, y=game.height/2, radius=100}
    earth_orbit     = node{x=100, spin=0.1}
    earth           = node{radius=50}
    moon_orbit      = node{x=20, spin=0.1}
    moon            = node{radius=25}
  )");

  SUBCASE("Node get/set")
  {
    RUN_LUA(lua, R"(
      sun.x = 20 
      sun.radius = 200
      sun.extra_info = { glow=true }

      assert(sun.radius == 200, "component added on init")
      assert(sun.extra_info ~= nil and sun.extra_info.glow == true, "component added post-init")
    )");
    Node sun = lua["sun"];
    int radius = lua["sun"]["radius"];
    sol::table extra_info = lua["sun"]["extra_info"];
    CHECK( radius == 200 );
    CHECK( extra_info["glow"] == true );
    CHECK( sun.x == 20 );
  }

  SUBCASE("children using add()")
  {
    RUN_LUA(lua, R"(
      game.scene:add(
        solar_system:add(
          sun,
          earth_orbit:add(
            earth,
            moon_orbit:add(
              moon
            )
          )
        )
      )
    )");
  }

  SUBCASE("children using +")
  {
    RUN_LUA(lua, R"(
      game.scene:add(
        solar_system + {
          sun,
          earth_orbit + {
            earth,
            moon_orbit +
              moon
          }
        }
      )
    )");
  }

  SUBCASE("clear children")
  {
    // CHECK_LUA(lua, R"(

    // )");
  }
}