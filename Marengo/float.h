#pragma once

#include "console.h"

#define FLOAT_PRECISION 3

typedef struct {
	int signum;
	int character;
	int mantisa;
	int precision;
} float_t;

float_t myatof(char* str);
int myftoa(float_t f, char* buf);
float_t idivide(int a, int b);
void printFloat(float_t f);
