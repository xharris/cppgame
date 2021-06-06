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

bool Engine::error(sol::protected_function_result &r)
{
  if (!r.valid())
  {
    sol::error err = r;
    // std::cerr << "Error: " << err.what() << std::endl;
    TraceLog(LOG_ERROR, err.what());

    // TODO: add custom user error-handling
    CloseWindow();

    return true;
  }
  return false;
}

bool Engine::error(std::initializer_list<const char *> error)
{
  std::ostringstream os_error;
  for (auto err : error)
  {
    os_error << err;
  }

  TraceLog(LOG_ERROR, os_error.str().c_str());
  return true;
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
    if (error(rs)) return true;
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
    "height", sol::readonly(&GameSetting::height),
    "scene", sol::readonly(&GameSetting::root_node)
  );
  
  lua["window"] = &Engine::window;
  lua["game"] = &Engine::game;
  SetTargetFPS(game.fps);

  lua.set_function("background", &Engine::background, this);
  Engine::game.background = black;

  return false;
}

bool Engine::load(sol::state& lua)
{
  // load main.lua
  auto r = lua.safe_script_file("main.lua", sol::script_pass_on_error);
  if (error(r)) return true;

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
  
  while (!WindowShouldClose())
  {
    // variable timestep (game)
    if (fn_update.valid())
    {
      auto ru = fn_update(GetFrameTime());
      error(ru);
    }

    // fixed timestep (physac automatically does this)
    // ...

    if (fn_draw.valid())
    {
      BeginDrawing();
      ClearBackground(Engine::game.background);
      auto rd = fn_draw();
      error(rd);
      EndDrawing();
    }
  }

  CloseWindow();
}

void Engine::background(Color c) { Engine::game.background = c; }