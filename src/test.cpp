// #define CATCH_CONFIG_MAIN
// #include <catch.hpp>

#include "test.h"

TEST_CASE("Color")
{
  // Engine e = Engine();
  Color c_rgb = rgb(0.1, 0.2, 0.3);
  Color c_rgba = rgb(0.1, 0.2, 0.3, 0.4);

  SUBCASE("rgb / rgba")
  {
    // init
    CHECK( c_rgba.r == (int)(0.1 * 255) );
    CHECK( c_rgba.g == (int)(0.2 * 255) );
    CHECK( c_rgba.b == (int)(0.3 * 255) );

    CHECK( c_rgba.r == (int)(0.1 * 255) );
    CHECK( c_rgba.g == (int)(0.2 * 255) );
    CHECK( c_rgba.b == (int)(0.3 * 255) );
    CHECK( c_rgba.a == (int)(0.4 * 255) );

    CHECK_LUA(R"(
      c_rgb = rgb(0.1, 0.2, 0.3);
      c_rgba = rgb(0.1, 0.2, 0.3, 0.4);

      assert(c_rgb == c_rgba);
    )");

    // equality
    Color c_rgba2 = rgb(0.1, 0.2, 0.3, 0.4);
    CHECK( c_rgba == c_rgba2 );
  }
}