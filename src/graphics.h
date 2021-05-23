#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <string.h>
#include <iostream>
#include <stack>
#include <unordered_map>
#include <vector>
#include <variant>
#include "engine.h"

typedef unsigned char uchar;

#define NUM_COLORS 3 // not including white/black
#define NUM_SHADES 6 // including color50

#define white       Color{255,255,255,255}
#define black       Color{0,0,0,255}
#define blank       Color{0,0,0,0}

#define grey50      Color{245,245,245,255}
#define grey100     Color{238,238,238,255}
#define grey200     Color{165,214,167,255}
#define grey300     Color{129,199,132,255}
#define grey400     Color{232,245,233,255}
#define grey500     Color{76,175,80,255}

#define green50     Color{232,245,233,255}
#define green100    Color{200,230,201,255}
#define green200    Color{165,214,167,255}
#define green300    Color{129,199,132,255}
#define green400    Color{232,245,233,255}
#define green500    Color{76,175,80,255}

#define blue50      Color{227,242,253,255}
#define blue100     Color{187,222,251,255}
#define blue200     Color{144,202,249,255}
#define blue300     Color{100,181,246,255}
#define blue400     Color{66,165,245,255}
#define blue500     Color{33,150,243,255}

struct GraphicsState {
  Color fill;
  Color stroke;
};

class GraphicsStack {
  public:
  GraphicsStack();
  ~GraphicsStack();
  GraphicsState* current;
  void push();
  void pop();
  void reset();

  private:
  std::stack<GraphicsState> stack;
};

// color 

inline bool operator == (const Color& lhs, const Color& rhs)
{
  return std::tie(lhs.r,lhs.g,lhs.b,lhs.a) == std::tie(rhs.r,rhs.g,rhs.b,rhs.a);
}

inline std::ostream& operator<<(std::ostream& os, const Color& c)
{
  return os << "{" << 
    (int)c.r << ", " << 
    (int)c.g << ", " << 
    (int)c.b << ", " << 
    (int)c.a << "}";
}

Color rgb(int r, int g, int b);
Color rgb(int r, int g, int b, int a);
Color hsv(float h, float s, float v);
Color hsv(float h, float s, float v, float a);
Color hex(int hex);
// Color hex(const char* hex);
Color alpha(Color c, float alpha);
Color fade(Color c, float alpha);

void fill();
void fill(Color c);
void stroke();
void stroke(Color c);

// text

void text(const char *text);
void text(const char *text, int x, int y);
void text(const char *text, int x, int y, int w, int h);

void font(sol::table f);

// image

struct SpriteInfo {
  std::vector<int> frames;
};

class BImage {
  public:
  BImage(const char* filename);
  void operator()();
  BImage* sprite(const char *, sol::table, int, int);

  static BImage create(const char* filename);

  private:
  bool changed = false;
  Image image;
  Texture2D texture;

  static std::unordered_map<const char*, SpriteInfo> sprite_info();
};

void bind_graphics(sol::state&);

#endif