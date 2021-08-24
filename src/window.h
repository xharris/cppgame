#ifndef WINDOW_H
#define WINDOW_H

#include <iostream>

#include "sol.h"
#include "raylib.hpp"

void window(sol::table w);

struct WindowSetting {
  int width = 800;
  int height = 450;
  const char* title = "Game";

  void operator()(sol::table t)
  {
    width = t["width"].get_or(width);
    height = t["height"].get_or(height);
    title = t["title"].get_or(title);

    if (IsWindowReady())
    {
      SetWindowSize(width, height);
    }
  };
};

void bind_window(sol::state&);

#endif 