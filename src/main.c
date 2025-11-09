#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "rom.h"
#include "cpu.h"
#include "debug.h"

int main(int argc, char *argv[])
{
  bool debug = false;

  if (argc < 2) {
    printf("Usage: %s <Path to .gb File> [-d DEBUG]\n", argv[0]);
    return 1;
  }
  if (argc == 3) {
    if (strcmp(argv[2], "-d") == 0 || strcmp(argv[2], "--debug") == 0) {
      printDebug("DEBUG MODE ON!", "");
      debug = true;
    }
  }

  FILE *file = fopen(argv[1], "rb");
  if (!file) {
    perror("Couldn't load ROM File");
    return 1;
  }

  fseek(file, 0, SEEK_END);
  long size = ftell(file);
  fseek(file, 0, SEEK_SET);
  unsigned char* rom = ReadFile(file, size);

  CPU cpu;
  cpu.A = 0;
  cpu.B = 0;
  cpu.C = 0;
  cpu.D = 0;
  cpu.E = 0;
  cpu.H = 0;
  cpu.L = 0;
  cpu.BC = 0;
  cpu.DE = 0;
  cpu.HL = 0;
  cpu.PC = 0;
  cpu.SP = 0;

  Run(rom, cpu, debug, size);

  return 0;
}
