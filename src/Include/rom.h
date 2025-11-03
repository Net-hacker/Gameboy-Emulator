#ifndef _ROM_H
#define _ROM_H

#define ROM_SIZE 4194304

#include <stdio.h>
#include <stdint.h>

extern unsigned char *rom;
extern long size;

//extern uint8_t rom;
//extern size_t bytes_read;

unsigned char* ReadFile(FILE *file);

#endif
