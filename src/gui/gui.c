#include <gui.h>

#include "raylib.h"

#include <stdio.h>
#include <stdlib.h>

const uint16_t WIDTH = 160;
const uint16_t HEIGHT = 144;

/**
 * Eine Fensterinstanz wird initiiert.
 *
 * Erstellt ein Fenster, spielt den Startup Sound ab und spielt die Startup Animation ab.
 */
void StartInstance()
{
  InitWindow(WIDTH, HEIGHT, "Gameboy Emulator");
  InitAudioDevice();
  SetMasterVolume(100);

  Sound startup = LoadSound("Sound/Startup.mp3");
  if (!IsSoundValid(startup)) {
    printf("Couldn't load Startup sound!\n");
    exit(1);
  }

  SetTargetFPS(60);

  const uint16_t fontSize = 16;
  const char *title = "Nintendo®";
  Vector2 titlePos = { (WIDTH - MeasureText(title, fontSize)) / 2, 0 };

  while ((titlePos.y + fontSize) <= (HEIGHT / 2) && !WindowShouldClose()) {
    BeginDrawing();
      DrawText(title, titlePos.x, titlePos.y, fontSize, BLACK);
      titlePos.y += 2;
      WaitTime(0.1);
      ClearBackground(WHITE);
    EndDrawing();
  }

  BeginDrawing();
    DrawText(title, titlePos.x, titlePos.y, fontSize, BLACK);
    PlaySound(startup);
    while (IsSoundPlaying(startup)) {
      WaitTime(0.1);
    }
    ClearBackground(WHITE);
  EndDrawing();
}
