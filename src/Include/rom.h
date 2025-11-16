#ifndef _ROM_H
#define _ROM_H

#include <cpu.h>

#include <stdio.h>
#include <stdint.h>

extern long size;

void ReadFile(FILE *file, long size, CPU cpu);

#endif
