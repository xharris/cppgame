#include "error.h"

bool Error::check(sol::protected_function_result &r)
{
  if (!r.valid())
  {
    sol::error err = r;
    // std::cerr << "Error: " << err.what() << std::endl;
    TraceLog(LOG_ERROR, err.what());

    // TODO: add custom user error-handling?
    CloseWindow();

    return true;
  }
  return false;
}

bool Error::throws(std::initializer_list<const char *> error)
{
  std::ostringstream os_error;
  for (auto err : error)
  {
    os_error << err;
  }

  TraceLog(LOG_ERROR, os_error.str().c_str());
  return true;
}