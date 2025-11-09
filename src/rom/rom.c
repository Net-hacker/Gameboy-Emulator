#include <rom.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * Einlesung der .gb Datei in eine Variable.
 *
 * Hier wird die .gb Datei eingelesen und in eine Variable gespeichert um damit weiterzuarbeiten.
 *
 * @param *file Die Datei als FILE.
 * @param size Die Größe der .gb Datei.
 * @return Eine unsigned char* Variable mit dem Inhalt der .gb Datei.
 */
unsigned char* ReadFile(FILE *file, long size)
{
  unsigned char *rom = (unsigned char*) malloc(size);
  if (rom == NULL) {
    printf("ERROR\n");
    exit(1);
  }

  fread(rom, 1, size, file);
  fclose(file);

  return rom;
}
