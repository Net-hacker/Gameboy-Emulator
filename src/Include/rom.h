#ifndef _ROM_H
#define _ROM_H

#define ROM_SIZE 248000

#include <stdio.h>
#include <stdint.h>

extern uint8_t rom[ROM_SIZE];

void ReadFile(FILE *file);

#endif
