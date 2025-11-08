#include <rom.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * Einlesung der .gb Datei in eine Variable.
 *
 * Hier wird die .gb Datei eingelesen und in eine Variable gespeichert um damit weiterzuarbeiten.
 *
 * @param *file Die Datei als FILE.
 * @return Eine unsigned char* Variable mit dem Inhalt der .gb Datei.
 */
unsigned char* ReadFile(FILE *file)
{
  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);

  unsigned char *rom = (unsigned char*) malloc(size);
  if (rom == NULL) {
    printf("ERROR\n");
    exit(1);
  }

  fread(rom, 1, size, file);
  fclose(file);

  return rom;
}
