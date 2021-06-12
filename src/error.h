#ifndef ERROR_H
#define ERROR_H

#include "sol.h"
#include "raylib.hpp"

namespace Error {
  // check the result of a lua function for errors
  bool check(sol::protected_function_result&);
  // manually show an error message
  bool throws(std::initializer_list<const char *>);
};

#endif 