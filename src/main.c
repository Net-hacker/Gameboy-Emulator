#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gui.h"
#include "rom.h"
#include "cpu.h"

int main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Verwendung: %s <Pfad zur .gb Datei>\n", argv[0]);
    return 1;
  }

  FILE *file = fopen(argv[1], "rb");
  if (!file) {
    perror("Konnte die ROM-Datei nicht öffnen");
    return 1;
  }

  //srand((unsigned) time(NULL));
  //float dt;

  unsigned char* rom = ReadFile(file);

  StartInstance();

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

  Run(rom, cpu);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
