#include <stdio.h>
#include <string.h>
#include <stdarg.h>

bool checkString(const char* a, const char* b)
{
  if (strcmp(a, b) == 0)
    return true;
  else
    return false;
}

void printDebug(const char* info, const char* format, ...)
{
  printf("\033[33;40;1m[Debug]:\033[0m %s", info);
  va_list args;
  va_start(args, format);

  vprintf(format, args);

  va_end(args);
  printf("\n");
}

void printError(const char* error, const char* format, ...)
{
  printf("\033[31;40;1mError\033[0m %s\n", error);
  va_list args;
  va_start(args, format);

  vprintf(format, args);

  va_end(args);
  printf("\n");
}
