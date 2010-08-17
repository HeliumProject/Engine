#pragma once

namespace Core
{
  char* StrStrip(const char* string);
  char* Strdup(const char* string);
  char* DirName(const char* path);
  
  void  Log(const char* format, ...);
  void  Warning(const char* format, ...);
  void  Error(const char* format, ...);
}

#ifdef DEBUG
#define DBG(_x) ((void)(_x))
#else
#define DBG(_x) ((void)0)
#endif
