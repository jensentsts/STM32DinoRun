#include "ctweeny_ease.h"

float ctweeny_linear(float processing, float start, float end){
    return start + (end - start) * processing;
}

float ctweeny_easeOutCubic(float processing, float start, float end){
	return start + (end - start) * (1 - pow(1 - processing, 3));
}

float ctweeny_easeInQuart(float processing, float start, float end){
	return start + (end - start) * (processing * processing * processing * processing);
}

float ctweeny_easeInOutBack(float processing, float start, float end){
	const float c1 = 1.70158;
	const float c2 = c1 * 1.525;

	return start + (end - start) * (
	processing < 0.5
		? (pow(2 * processing, 2) * ((c2 + 1) * 2 * processing - c2)) / 2
		: (pow(2 * processing - 2, 2) * ((c2 + 1) * (processing * 2 - 2) + c2) + 2) / 2);
}

float ctweeny_easeOutElastic(float processing, float start, float end){
	const float c4 = (2 * CTWEENY_PI) / 3;

	return start + (end - start) * (
		processing == 0
			? 0
			: processing == 1
				? 1
				: pow(2, -10 * processing) * sin((processing * 10 - 0.75) * c4) + 1);
}


float ctweeny_easeInOutQuint(float processing, float start, float end){
	return start + (end - start) * (
		processing < 0.5 
			? 16 * processing * processing * processing * processing * processing 
			: 1 - pow(-2 * processing + 2, 5) / 2);
}
