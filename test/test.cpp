#include "test.h"
#include <iostream>
#include <iomanip>

TEST_CASE("Graphics")
{
  sol::state lua;
  Engine e = useLua(lua);

  SUBCASE("Color")
  {
    RUN_LUA(lua, R"(
      c_rgb = rgb(10, 20, 30)
      c_rgba = rgb(10, 20, 30, 40)
      
      -- equivalent to c_rgb
      c_hsv = hsv(210, .667, .118)  
      c_hex = hex(0x0a141e)      
    )");

    SUBCASE("rgb(a)")
    {
      Color c_rgb = lua["c_rgb"];
      Color c_rgba = lua["c_rgba"];
      CHECK(c_rgb.r == 10);
      CHECK(c_rgb.g == 20);
      CHECK(c_rgb.b == 30);
      CHECK(c_rgb.a == 255);
      CHECK(c_rgba.a == 40);

      RUN_LUA(lua, "c_rgb.a = 40");
      c_rgb = lua["c_rgb"];
      CHECK(c_rgb == c_rgba);
    }

    SUBCASE("hsv")
    {
      CHECK((Color)lua["c_rgb"] == (Color)lua["c_hsv"]);
    }

    SUBCASE("hex")
    {
      CHECK((Color)lua["c_rgb"] == (Color)lua["c_hex"]);
    }
  }

  SUBCASE("Settings")
  {
    RUN_LUA(lua, R"(
      c_rgb = rgb(10, 20, 30)
      fill(c_rgb)
      stroke(c_rgb)
    )");
    Color c_rgb = lua["c_rgb"];
    CHECK( Engine::graphics.current->fill == c_rgb );
    CHECK( Engine::graphics.current->stroke == c_rgb );
  }
}

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