#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

/**
 * Überprüfung ob zwei Strings gleich sind.
 *
 * Ich wollte einfach eine Möglichkeit zum Checken, weil es mir zu repetitiv wurde.
 *
 * @param a Die erste String.
 * @param b Die zweite String.
 * @return Ob die beiden Strings gleich sind oder nicht (true/false).
 */
bool checkString(const char* a, const char* b)
{
  if (strcmp(a, b) == 0)
    return true;
  else
    return false;
}

/**
 * Formatierte Ausgabe einer Meldung.
 *
 * Eine Debugmöglichkeit, falls Meldungen ausgegeben werden sollen.
 *
 * @param info Der Inhalt der Meldung-
 * @param format Hier und danach ist es wie bei printf.
 */
void printDebug(const char* info, const char* format, ...)
{
  printf("\033[33;40;1m[Debug]:\033[0m %s", info);
  va_list args;
  va_start(args, format);

  vprintf(format, args);

  va_end(args);
  printf("\n");
}

/**
 * Formatierte Ausgabe einer Fehlermeldung.
 *
 * Eine Debugmöglichkeit, falls Fehler auftreten sollten.
 *
 * @param error Die Fehlermeldungen, welche ausgegeben werden soll.
 * @param format Hier und danach ist es wie bei printf.
 */
void printError(const char* error, const char* format, ...)
{
  printf("\033[31;40;1mError\033[0m %s\n", error);
  va_list args;
  va_start(args, format);

  vprintf(format, args);

  va_end(args);
  printf("\n");
}
