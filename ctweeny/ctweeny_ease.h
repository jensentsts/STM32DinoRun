#ifndef __CTWEENY_EASE_H
#define __CTWEENY_EASE_H

#include <math.h>

#define CTWEENY_PI 3.1415926

float ctweeny_linear(float processing, float start, float end);
float ctweeny_easeOutCubic(float processing, float start, float end);
float ctweeny_easeInQuart(float processing, float start, float end);
float ctweeny_easeInOutBack(float processing, float start, float end);
float ctweeny_easeOutElastic(float processing, float start, float end);
float ctweeny_easeInOutQuint(float processing, float start, float end);

#endif
