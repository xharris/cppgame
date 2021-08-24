#ifndef ENGINE_H
#define ENGINE_H

#include <initializer_list>
#include <sstream>

#include "sol.h"
#include "raylib.hpp"
#include "uuid.h"
#include "graphics.h"
#include "window.h"
#include "error.h"
#include "fs.h"

struct GameSetting
{
  Color background;
  int fps = 60;
  int width = 800;
  int height = 450;

  float font_size = 12;
  float font_spacing = 0;
  bool font_wrap = false;

  void operator()(sol::table t)
  {
    fps = t["fps"].get_or(fps);

    SetTargetFPS(fps);
  };

  void quit()
  {
    CloseWindow();
  }
};

class GraphicsStack;

class Engine {
  public:
  static struct GameSetting game;
  static struct WindowSetting window;
  static GraphicsStack graphics;

  // METHODS

  void go();
  void initLua(sol::state&);
  bool bind(sol::state&);

  private:
  bool fnExists(sol::state&, const char*);
  bool call(sol::state&, const char*);
  bool load(sol::state&);
  void loop(sol::state&);
};

#endif