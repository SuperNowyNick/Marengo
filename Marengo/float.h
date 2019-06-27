#pragma once

#include "console.h"

#define FLOAT_PRECISION 3

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
float_t fzero(void);
void printFloat(float_t f);
