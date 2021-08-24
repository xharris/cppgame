#ifndef FS_H
#define FS_H

#include "sol.h"
#include "raylib.hpp"
#include <string.h>
#include "dirent.h"
#include <vector>
#include <map>

sol::as_table_t<std::vector<std::string>> listDir(const char *path);
bool isFile(const char *path);
bool isDirectory(const char *path);
const char * getCwd();
bool cd(const char* dir);

void bind_fs(sol::state&);

#endif