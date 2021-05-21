#include "window.h"

void bind_window(sol::state& lua)
{
  lua.new_usertype<WindowSetting>("WindowSetting",
    "width", sol::readonly(&WindowSetting::width),
    "height", sol::readonly(&WindowSetting::height)
  );
}