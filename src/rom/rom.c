#include <rom.h>
#include <cpu.h>

#include <stdio.h>
#include <stdlib.h>

/**
 * Einlesung der .gb Datei in eine Variable.
 *
 * Hier wird die .gb Datei eingelesen und in eine Variable gespeichert um damit weiterzuarbeiten.
 *
 * @param *file Die Datei als FILE.
 * @param size Die Größe der .gb Datei.
 * @param cpu Die CPU und ihre Register, Program Counter, Stack Pointer und Flaggen.
 */
void ReadFile(FILE *file, long size, CPU cpu)
{
  if (cpu.rom == NULL) {
    printf("ERROR\n");
    exit(1);
  }

  fread(cpu.rom, 1, size, file);
  fclose(file);
}
