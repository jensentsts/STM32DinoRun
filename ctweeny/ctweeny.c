#include "stm32f10x.h"                  // Device header
#include "ctweeny.h"

void ctweeny_init(ctweeny* __self__, float from, float to, float during){
    __self__->from = from;
    __self__->to = to;
    __self__->during = during;
    __self__->processing = 0;
    __self__->direction = foreward;
    __self__->viaFunction = ctweeny_linear;
    __self__->onStepFunction = 0;
}

void ctweeny_from(ctweeny* __self__, float val){
    __self__->from = val;
}

void ctweeny_to(ctweeny* __self__, float val){
    __self__->to = val;
}

void ctweeny_during(ctweeny* __self__, float val){
    __self__->during = val;
}

void ctweeny_via(ctweeny* __self__, float (*func)(float processing, float start, float end)){
	if (func == 0){
		return;
	}
    __self__->viaFunction = func;
}

void ctweeny_onStep(ctweeny* __self__, char (*func)(float val)){
	if (func == 0){
		return;
	}
    __self__->onStepFunction = func;
}

float ctweeny_val(ctweeny* __self__){
    if (__self__->viaFunction){
        return __self__->viaFunction(__self__->processing / __self__->during, __self__->from, __self__->to);
	}
	return 0;
}

#ifdef __cplusplus
inline
#endif
float ctweeny_process(ctweeny* __self__){
	return __self__->processing;
}

float ctweeny_step(ctweeny* __self__, float step){
    static float res = 0;
    __self__->processing += step * __self__->direction;
    if (__self__->processing > __self__->during){
        __self__->processing = __self__->during;
    }
    if (__self__->viaFunction){
        res = __self__->viaFunction(__self__->processing / __self__->during, __self__->from, __self__->to);
        if (__self__->onStepFunction){
            if (__self__->onStepFunction(res) != 0){
                __self__->onStepFunction = 0;
            }
        }
        return res;
    }
    else{
        return 0;
    }
}

float ctweeny_seek(ctweeny* __self__, float seek){
    __self__->processing = seek;
	if (__self__->processing > __self__->during){
		__self__->processing = __self__->during;
	}
	if (__self__->processing < 0){
		__self__->processing = 0;
	}
	if (__self__->viaFunction){
		return __self__->viaFunction(__self__->processing / __self__->during, __self__->from, __self__->to);
	}
	return 0;
}

float ctweeny_jump(ctweeny* __self__, float jump){
    __self__->processing = jump;
	if (__self__->processing > __self__->during){
		__self__->processing = __self__->during;
	}
	if (__self__->processing < 0){
		__self__->processing = 0;
	}
	if (__self__->viaFunction){
		return __self__->viaFunction(__self__->processing / __self__->during, __self__->from, __self__->to);
	}
	return 0;
}

void ctweeny_foreward(ctweeny* __self__){
    __self__->direction = foreward;
}

void ctweeny_backward(ctweeny* __self__){
    __self__->direction = backward;
}

ctweeny_Direction_Enum ctweeny_direction(ctweeny *__self__){
    return __self__->direction;
}
