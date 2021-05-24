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
    )");
    Color c_rgb = lua["c_rgb"];
    CHECK_LUA(lua, R"(
      fill(c_rgb)
    )");
    CHECK( Engine::graphics.current->fill == c_rgb );
  }
}