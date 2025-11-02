#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "gui.h"
#include "rom.h"

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

  srand((unsigned) time(NULL));
  float dt;

  StartInstance();

  ReadFile(file);

  fclose(file);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
