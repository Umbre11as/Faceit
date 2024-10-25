#include "Math.h"

// https://codelessons.dev/ru/cos-in-c-cplusplus/

float MthPow(IN float Base, IN int Exponent) {
    float result = 1;
    for (int i = 0; i < Exponent; i++)
        result *= Base;

    return result;
}

float MthSign(IN float X) {
    if (X > 0)
        return 1;
    else if (X < 0)
        return -1;
    else
        return 0;
}

int MthFactorial(IN int N) {
    if (N == 0)
        return 1;
    else
        return N * MthFactorial(N - 1);
}

float MthSin(IN float X) {
    while (X >= 2 * M_PI)
        X -= MthSign(X) * 2 * M_PI;

    float result = X;

    for (int i = 1; i <= 4; i++) {
        float term = MthPow(X, 1 + 2 * i) / MthFactorial(1 + 2 * i);
        if (i % 2 == 1)
            result -= term;
        else
            result += term;
    }

    return result;
}

float MthCos(IN float X) {
    while (X >= 2 * M_PI)
        X -= MthSign(X) * 2 * M_PI;

    float result = 1;

    for (int i = 1; i <= 4; i++) {
        float term = MthPow(X, 2 * i) / MthFactorial(2 * i);
        if (i % 2 == 1)
            result -= term;
        else
            result += term;
    }

    return result;
}

float MthAtan(IN float X) {
    float result = X;

    for (int i = 1; i <= 4; i++) {
        float term = MthPow(X, 2 * i - 1) / (2 * i - 1);
        if (i % 2 == 1)
            result -= term;
        else
            result += term;
    }

    return result;
}

float MthAtan2(IN float Y, IN float X) {
    if (X == 0) {
        if (Y > 0)
            return M_PI / 2;
        else if (Y < 0)
            return -M_PI / 2;
        else
            return 0;
    } else {
        float theta = MthAtan(Y / X);
        if (X > 0)
            return theta;
        else if (Y >= 0)
            return theta + M_PI;
        else
            return theta - M_PI;
    }
}

float MthAbs(IN float X) {
    if (X >= 0)
        return X;
    else
        return -X;
}

float MthSqrt(IN float X) {
    if (X < 0.0f)
        return 0.0f;

    float guess = 1.0f;

    for (int i = 0; i < 10; i++)
        guess = (guess + X / guess) * 0.5f;

    return guess;
}

float MthFloor(IN float X) {
    if (X >= 0)
        return (float)(int)X;
    else
        return (float)(int)X - 1;
}

float MthCeil(IN float X) {
    if (X >= 0)
        return (double)(int)X + (X - (int)X > 0);
    else
        return (double)(int)X;
}
