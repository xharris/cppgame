#include "graphics.h"

GraphicsStack::GraphicsStack()
{
  stack.push(GraphicsState{white, white});
  current = &stack.top();
}
GraphicsStack::~GraphicsStack()
{
  delete current;
}
// push a copy of the current state on the stack
void GraphicsStack::push() 
{ 
  stack.push(stack.top()); 
  current = &stack.top();
}
void GraphicsStack::pop() 
{
  if (stack.size() > 1)
    stack.pop();
  current = &stack.top();
}
void GraphicsStack::reset()
{
  while (!stack.empty()) stack.pop();
  stack.push(GraphicsState{});
  current = &stack.top();
}

Color rgb(int r, int g, int b)
{
  return Color{(uchar)r, (uchar)g, (uchar)b, (uchar)255};
}
Color rgb(int r, int g, int b, int a) 
{ 
  return Color{(uchar)r, (uchar)g, (uchar)b, (uchar)a};
}
Color hsv(float h, float s, float v)
{
  return ColorFromHSV(h,s,v); 
}
Color hsv(float h, float s, float v, float a) 
{  
  return ColorAlpha(ColorFromHSV(h,s,v), a);
}
Color hex(int h) 
{ 
  // reformat hex value
  Color c{0,0,0,0};
  if (h == 0x0)
    return c;
  bool is_short = !(h >> 8 > 0x0);
  bool has_alpha = h >> 24 > 0x0;
  if (is_short)
    h = (h << 24) | 0x000000FF;
  else if (!has_alpha)
    h = (h << 8) | 0x000000FF;
  // convert to rgba
  c.r = (h >> 24) & 0xFF;
  c.g = (h >> 16) & 0xFF;
  c.b = (h >> 8) & 0xFF;
  c.a = h & 0xFF;
  return c;
}
// Color color(const char* hex) { return white; }

void fill() { Engine::graphics.current->fill = blank; }
void fill(Color c) { Engine::graphics.current->fill = c; }
void stroke() { Engine::graphics.current->stroke = blank; }
void stroke(Color c) { Engine::graphics.current->stroke = c; }

void text(const char *text)
{
  DrawText(text, 0, 0, Engine::game.font_size, Engine::graphics.current->fill);
}
void text(const char *text, int x, int y)
{
  DrawText(text, x, y, Engine::game.font_size, Engine::graphics.current->fill);
}
void text(const char *text, float x, float y, float w, float h)
{
  DrawTextRec(GetFontDefault(), text, Rectangle{x,y,w,h}, Engine::game.font_size, Engine::game.font_spacing, Engine::game.font_wrap, Engine::graphics.current->fill);
}

void font(sol::table f)
{
  Engine::game.font_size = f["size"].get_or(Engine::game.font_size);
  Engine::game.font_spacing = f["spacing"].get_or(Engine::game.font_spacing);
  Engine::game.font_wrap = f["wrap"].get_or(Engine::game.font_wrap);
}

BImage::BImage(const char* filename)
{
  if (!FileExists(filename))
    Engine::error({"Image not found: ", filename});
  else 
  {
    image = LoadImage(filename);
    changed = true;
  }
}

void BImage::operator()() {
  if (changed || texture.id == 0)
  {
    texture = LoadTextureFromImage(image);
    changed = false;
  }
  DrawTexture(texture, 0, 0, Engine::graphics.current->fill);
}

BImage BImage::create(const char* filename)
{
  return BImage(filename);
}

BImage* BImage::sprite(const char * name, sol::table frames, int framew, int frameh)
{
  std::vector<int> vec_frames;
  frames.for_each([&](sol::object const& k, sol::object const& v)
  { // parse frames
    if (v.get_type() == sol::type::number)
    { // int 
      vec_frames.push_back(v.as<int>());
    }
    else if (v.get_type() == sol::type::string)
    { // const char *
      std::string str_frame(v.as<std::string>());
      int f_len = str_frame.length();
      
      for (int i = 0; i < f_len; i++)
      {

      }
    }
  });
  return this;
}

void bind_graphics(sol::state& lua)
{
  lua.set_function("push", &GraphicsStack::push, &Engine::graphics);
  lua.set_function("pop", &GraphicsStack::pop, &Engine::graphics);
  lua.set_function("reset", &GraphicsStack::reset, &Engine::graphics);

  // color

  lua.new_usertype<Color>("Color",
    "r", &Color::r,
    "g", &Color::g,
    "b", &Color::b,
    "a", &Color::a
  );

  Color colors[] = {
    grey50, grey100, grey200, grey300, grey400, grey500,
    green50, green100, green200, green300, green400, green500,
    blue50, blue100, blue200, blue300, blue400, blue500
  };

  const char *color_names[] = COLOR_STRINGS;

  std::string str_color;
  for (int c = 0; c < NUM_COLORS; c++) // lol!
  {
    for (int shade = 0; shade < NUM_SHADES; shade++)
    {
      str_color = color_names[c];
      str_color += std::to_string(shade == 0 ? 50 : shade * 100);
      lua[str_color.c_str()] = colors[c * NUM_SHADES + shade];
      
      if (shade == NUM_SHADES / 2)
        lua[color_names[c]] = colors[c * NUM_SHADES + shade];
    }
  }
  
  lua["white"] = white;
  lua["black"] = black;
  lua["blank"] = blank;

  lua.set_function("rgb", sol::overload(
    sol::resolve<Color(int,int,int)>(rgb),
    sol::resolve<Color(int,int,int,int)>(rgb)
  ));
  lua.set_function("hsv", sol::overload(
    sol::resolve<Color(float,float,float)>(hsv),
    sol::resolve<Color(float,float,float,float)>(hsv)
  ));
  lua.set_function("hex", hex);
  
  lua.set_function("fill", sol::overload(
    sol::resolve<void()>(fill),
    sol::resolve<void(Color)>(fill)
  ));
  lua.set_function("stroke", sol::overload(
    sol::resolve<void()>(stroke),
    sol::resolve<void(Color)>(stroke)
  ));

  // text 
  
  lua.set_function("text", sol::overload(
    sol::resolve<void(const char*)>(text), 
    sol::resolve<void(const char*, int, int)>(text), 
    sol::resolve<void(const char*, float, float, float, float)>(text)
  ));

  lua.set_function("font", font);

  // image 

  sol::usertype<BImage> img_type = lua.new_usertype<BImage>("image",
    sol::call_constructor,
    sol::factories([](const char* filename)
    {
      return BImage(filename);
    })
  );
  img_type.set_function("sprite", &BImage::sprite);
  // img_type["sprite"] = &BImage::sprite;
}