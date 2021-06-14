#include "engine.h"

struct WindowSetting Engine::window;
struct GameSetting Engine::game;
GraphicsStack Engine::graphics = GraphicsStack();

void Engine::go()
{
  SetTraceLogLevel(LOG_DEBUG);
  sol::state lua;
  initLua(lua);
  if (!bind(lua) && !load(lua))
    loop(lua);
}

bool Engine::fnExists(sol::state& lua, const char* fn_name)
{
  sol::object fn = lua[fn_name];
  return fn.get_type() == sol::type::function;
}

bool Engine::call(sol::state& lua, const char* fn_name)
{
  TraceLog(LOG_INFO, "calling %s", fn_name);
  sol::function fn = lua[fn_name];
  if (fn.valid())
  {
    auto rs = fn();
    if (Error::check(rs)) return true;
  }
  return false;
}

void Engine::initLua(sol::state& lua)
{
  lua.open_libraries(sol::lib::base, sol::lib::package);
}

bool Engine::bind(sol::state &lua)
{
  bind_ecs(lua);
  bind_graphics(lua);
  bind_window(lua);

  lua.new_usertype<GameSetting>("GameSetting",
    "fps", sol::readonly(&GameSetting::fps),
    "width", sol::readonly(&GameSetting::width),
    "height", sol::readonly(&GameSetting::height)
  );
  
  lua["window"] = &Engine::window;
  lua["game"] = &Engine::game;
  SetTargetFPS(game.fps);
  
  lua.set_function("background", sol::overload(
    [&](Color c) { this->game.background = c; },
    [&]() -> Color { return this->game.background; }
  ));
  Engine::game.background = black;

  return false;
}

bool Engine::load(sol::state& lua)
{
  // load main.lua
  auto r = lua.safe_script_file("main.lua", sol::script_pass_on_error);
  if (Error::check(r)) return true;

  if (fnExists(lua, "setup"))
  {
    InitWindow(
      Engine::window.width, Engine::window.height, 
      Engine::window.title
    );

    if (call(lua, "setup")) return true;
  }
  return false;
}

void Engine::loop(sol::state& lua)
{
  sol::function fn_update = lua["update"];
  sol::function fn_draw = lua["draw"];
  // sol::function fn_system_draw = sol::as_function(&System::drawAll);
  
  while (!WindowShouldClose())
  {
    // variable timestep (game)
    if (fn_update.valid())
    {
      auto ru = fn_update(GetFrameTime());
      Error::check(ru);
    }
    // update ecs systems
    System::updateAll(GetFrameTime());

    // fixed timestep (physac automatically does this)
    // ...

    BeginDrawing();
    ClearBackground(Engine::game.background);
    if (fn_draw.valid())
    {
      auto rd = fn_draw();
      Error::check(rd);
    }
    System::drawAll();
    EndDrawing();
  }

  CloseWindow();
}