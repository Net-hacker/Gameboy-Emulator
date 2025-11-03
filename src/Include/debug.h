#ifndef _DEBUG_H
#define _DEBUG_H

#include <stdbool.h>

bool checkString(const char* a, const char* b);
void printDebug(const char* info, const char* format, ...);
void printError(const char* error, const char* format, ...);

#endif
