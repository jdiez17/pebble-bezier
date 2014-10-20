#ifndef _BEZIER_H
#define _BEZIER_H

#include <pebble.h>

// Auxiliary functions
int fact(int n);
float fpow(float n, int exp);
int binomialCoefficient(int n, int k);

// The main thing
GPoint lerpBezier(GPoint* points_a, GPoint* points_b, float t, float lerp, int path, int n, int xoff, int yoff);
void renderBezierDigit(GContext* ctx, int digit, float t_lerp, int xoff, int yoff, bool should_lerp);

#endif
