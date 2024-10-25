#pragma once

#define MAX(x, y) x > y ? x : y
#define MIN(x, y) x < y ? x : y
#define CLAMP(x, min, max) MIN(MAX(x, min), max)

#define M_PI 3.14159f

#define DEG2RAD(x) (x * M_PI / 180.0f)
#define RAD2DEG(x) (x * 180.0f / M_PI)

#include "Math.h"

float MthPow(IN float Base, IN int Exponent);

float MthSign(IN float X);

int MthFactorial(IN int N);

float MthSin(IN float X);

float MthCos(IN float X);

float MthAtan(IN float X);

float MthAtan2(IN float Y, IN float X);

float MthAbs(IN float X);

float MthSqrt(IN float X);

float MthFloor(IN float X);

float MthCeil(IN float X);
