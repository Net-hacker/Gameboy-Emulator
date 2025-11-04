#include <rom.h>

#include <stdio.h>
#include <stdlib.h>

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
