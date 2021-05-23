// #define CATCH_CONFIG_MAIN
// #include <catch.hpp>

#include "test.h"

TEST_CASE("Color")
{
  sol::state lua;
  useLua(lua);

  SUBCASE("rgb(a)")
  {
    CHECK_LUA(lua, R"(
      c_rgb = rgb(10, 20, 30)
      c_rgba = rgb(10, 20, 30, 40)
    )");

    CHECK_LUA(lua, R"(
      assert(c_rgb.r == 10, "red value")
      assert(c_rgb.g == 20, "green value")
      assert(c_rgb.b == 30, "blue value")
      assert(c_rgb.a == 40, "alpha value")

      c_rgb.a = 40
      assert(c_rgb == c_rgba, "equality")
    )");

  }
}