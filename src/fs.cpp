#include "fs.h"

sol::as_table_t<std::vector<std::string>> listDir(const char *path)
{
  int count;
  char** files = GetDirectoryFiles(path, &count);
  std::vector<std::string> ret;
  int f = 0, r = 1;
  // while (f < count)
  // {
  //   if (strcmp(files[f], ".") * strcmp(files[f], "..") != 0)
  //   {
  //     ret[r++] = files[f];
  //   }
  //   f++;
  // }
  DIR *dir;
  struct dirent *ent;
  if ((dir = opendir(path)) != NULL)
  {
    while ((ent = readdir(dir)) != NULL)
    {
      if (strcmp(ent->d_name, ".") * strcmp(ent->d_name, "..") != 0)
      {
        ret.push_back(ent->d_name);
      }
    }
    closedir(dir);
  } else TraceLog(LOG_WARNING, "FILEIO: Failed to open requested directory");
  return sol::as_table(ret);
}
bool isFile(const char *path)
{
  return FileExists(path) && !DirectoryExists(path);
}
bool isDirectory(const char *path)
{
  return DirectoryExists(path);
}
const char * getCwd()
{
  return GetWorkingDirectory();
}
bool cd(const char* dir)
{
  return ChangeDirectory(dir);
}

void bind_fs(sol::state& lua)
{
  sol::table fs = lua.create_table();

  // fs["ls"] = [](const char *path) -> sol::as_table_t<std::vector<std::string>> { 
  //   return sol::as_table(listDir(path)); 
  // };
  fs["ls"] = &listDir;
  fs["isFile"] = &isFile;
  fs["isDirectory"] = &isDirectory;
  fs["cwd"] = &getCwd;
  fs["cd"] = &cd;

  lua["fs"] = fs;
}