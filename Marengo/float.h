#pragma once

#include "console.h"

#define FLOAT_PRECISION 5

typedef struct {
    signed char signum;
    unsigned int character;
    unsigned int mantisa;
    signed char precision;
} float_t;

float_t myatof(char* str);
char* myftoa(float_t f, char* buf);
float_t itof(int a);
float_t idiv(int a, int b, signed char precision);
float_t fidiv(float_t a, int b, signed char precision);
float_t fadd(float_t a, float_t b);
float_t fsub(float_t a, float_t b);
float_t fmulti(float_t a, int b);
// TODO Add floats multiplication with carry flag
float_t fabs(float_t a);
float_t fzero(void);
float_t fcmp(float_t a, float_t b);
float_t fmax(int argc, ...);
int ffloor(float_t a);
int fpos(float_t a);
int fneg(float_t a);
int fnonzero(float_t a);
void printFloat(float_t f);
uint32_t isqrt64(uint64_t op);
uint16_t isqrt32(uint32_t op);
