#include "rom.h"

#include <stdio.h>
#include <stdlib.h>

void ReadFile(FILE *file)
{
  uint8_t rom[ROM_SIZE];
  size_t bytes_read = fread(rom, 1, ROM_SIZE, file);

  FILE *out = fopen("output.txt", "w");

  fprintf(out, "%05X  ", 0);
  for (size_t i = 0; i < bytes_read; ++i) {
    fprintf(out, "%02x ", rom[i]);
    if ((i + 1) % 16 == 0) {
      fprintf(out, "\n%05X  ", i);
    } else if ((i + 1) % 8 == 0) {
      fprintf(out, "  ");
    }
  }

  fclose(out);
}
