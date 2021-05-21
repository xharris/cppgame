// #define CATCH_CONFIG_MAIN
// #define SOL_CHECK_ARGUMENTS
// #include <catch.hpp>

// #include "engine.h"

// #define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
// #include "doctest.h"

#include <unordered_map>
#include <iostream>

#include "uuid.h"

typedef const char* propid;
typedef uuid::uuid nodeid;
template <typename K,typename V> 
using umap = std::unordered_map<K,V>;

class Pants {
public:
  int value;
  Pants() {};
  // Pants(const Pants &) = delete;
  Pants(int v) : value(v) {};
};

int main(int argc, char** argv)
{
  umap<propid, umap<nodeid, Pants>> mymap;
  mymap["bob"][1234] = Pants(20);
  std::cout << "bob's pants are a " << mymap["bob"][1234].value << std::endl;
}

// TEST_CASE("Color")
// {
//   Engine e = Engine();

//   SUBCASE("rgba")
//   {
//     Color c = e.color(0,0,0,0);
//     CHECK( c.r == 0 );
//     CHECK( c.g == 0 );
//     CHECK( c.b == 0 );
//     CHECK( c.a == 0 );
//   }
// }