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