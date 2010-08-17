#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

#include "Util.h"

using namespace Core;

/* strdup is actually not a standard ANSI C or POSIX routine
 so implement a private one for Core.  OpenVMS does not have a
 strdup; Linux's standard libc doesn't declare strdup by default
 (unless BSD or SVID interfaces are requested). */
char * Core::Strdup(const char *string)
{
  char *copy = (char*) malloc(strlen(string) + 1);
  if (copy)
  {
    strcpy(copy, string);
  }
  return copy;
}

/* strip leading and trailing whitespace from a string and return a newly
 allocated string containing the result (or NULL if the string is only 
 whitespace)*/
char * Core::StrStrip(const char *s)
{
  int first;
  int last = strlen(s)-1;
  int len;
  int i;
  char * rets;
  
  i=0;
  while(i <= last && (s[i]==' ' || s[i]=='\t' || s[i]=='\n' || s[i]=='\r'))
    i++;
  
  if (i>last)
    return NULL;
  
  first = i;
  i = last;
  
  while(i > first && (s[i]==' ' || s[i]=='\t' || s[i]=='\n' || s[i]=='\r'))
    i--;
  
  last = i;
  len = last-first+1;
  rets = (char*)malloc(len+1); /* add a trailing 0 */
  memcpy(rets, s + first, len);
  rets[len] = 0;
  return rets;
}

/* DirName: return the directory given a path
 *
 * path - filesystem path
 *
 * NOTE: the return value should be free'd.
 */
char* Core::DirName(const char* path)
{
  char* dir = Strdup(path);
  
  char* s = strrchr(dir, '/');
  if (s)
  {
    s[1] = '\0';
  }
  else
  {
    dir[0] = '\0';
  }
  
  return dir;
}

void Core::Log(const char *format,...)
{
  va_list args;
  va_start(args, format);
  vfprintf(stdout, format, args);
  va_end(args);
  
  putc('\n', stdout);
}

void Core::Warning(const char *format,...)
{
  va_list args;
  va_start(args, format);
  fprintf(stderr, "Warning: ");
  vfprintf(stderr, format, args);
  va_end(args);
  
  putc('\n', stderr);
}

void Core::Error(const char *format,...)
{
  va_list args;
  va_start(args, format);
  fprintf(stderr, "Error: ");
  vfprintf(stderr, format, args);
  va_end(args);
  
  putc('\n', stderr);
}