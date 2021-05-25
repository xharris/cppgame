#include "test.h"
#include <iostream>
#include <iomanip>

TEST_CASE("Graphics")
{
  sol::state lua;
  Engine e = useLua(lua);

  SUBCASE("Color")
  {
    CHECK_LUA(lua, R"(
      c_rgb = rgb(10, 20, 30)
      c_rgba = rgb(10, 20, 30, 40)
      c_hsv = hsv(210, .667, .118)

      c_hex = hex(0x0a141e)
    )");

    SUBCASE("rgb(a)")
    {
      CHECK_LUA(lua, R"(
        assert(c_rgb.r == 10, "red value")
        assert(c_rgb.g == 20, "green value")
        assert(c_rgb.b == 30, "blue value")
        assert(c_rgb.a == 255, "alpha value")
        assert(c_rgba.a == 40, "non-default alpha value")

        c_rgb.a = 40
        assert(c_rgb == c_rgba, "rgb-rgb equality")
      )");
    }

    SUBCASE("hsv")
    {
      CHECK_LUA(lua, R"(
        assert(c_rgb == c_hsv, "rgb-hsv equality")
      )");
    }

    SUBCASE("hex")
    {
      CHECK_LUA(lua, R"(
        assert(c_rgb == c_hex, "rgb-hex equality")
      )");
    }
  }

  SUBCASE("Settings")
  {
    CHECK_LUA(lua, R"(
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
  
  CHECK_LUA(lua, R"(
    solar_system    = node()
    sun             = node{x=game.width/2, y=game.height/2, radius=100}
    earth_orbit     = node{x=100, spin=0.1}
    earth           = node{radius=50}
    moon_orbit      = node{x=20, spin=0.1}
    moon            = node{radius=25}
  )");

  SUBCASE("Node get/set")
  {
    CHECK_LUA(lua, R"(
      sun.x = 20 
      sun.radius = 200
      sun.extra_info = { glow=true }

      assert(sun.radius == 200, "component added on init")
      assert(sun.extra_info ~= nil and sun.extra_info.glow == true, "component added post-init")
    )");
    Node sun = lua["sun"];
    CHECK( sun.x == 20 );
  }


}