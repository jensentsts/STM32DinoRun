/**
  * @file ctweeny.h
  * @brief Ctweeny.h 是仿照 tweeny 开发的c语言库。
           它的特点是它本身不会占用额外的内存空间，即不含开辟内存空间等操作，因此可以很好地在stm32、stc51等单片机上使用。
  * @note 过渡曲线请查看 ctweeny_ease.h
  */
#ifndef __CTWEENY_H
#define __CTWEENY_H

#include "ctweeny_ease.h"

typedef enum{
    foreward = 1,
    backward = -1
}ctweeny_Direction_Enum;

/**
  * @brief ctweeny结构体是 ctweeny 的基本类型，也是开发者需要用到的唯一的数据类型，作用相当于 tweeny 中的tween类
  * @warning 修改ctweeny结构体中的数据请瞳过调用 ctweeny 中的函数。
  */
typedef struct{
    float from, to;
    float during, processing;
    ctweeny_Direction_Enum direction;
    float (*viaFunction)(float processing, float start, float end);
    char (*onStepFunction)(float val);
}ctweeny;

void ctweeny_init(ctweeny* __self__, float from, float to, float during);
void ctweeny_from(ctweeny* __self__, float val);
void ctweeny_to(ctweeny* __self__, float val);
void ctweeny_during(ctweeny* __self__, float val);
void ctweeny_via(ctweeny* __self__, float (*func)(float processing, float start, float end));
void ctweeny_onStep(ctweeny* __self__, char (*func)(float val));

float ctweeny_val(ctweeny* __self__);
float ctweeny_process(ctweeny* __self__);

float ctweeny_step(ctweeny* __self__, float step);
float ctweeny_seek(ctweeny* __self__, float seek);
float ctweeny_jump(ctweeny* __self__, float jump);

void ctweeny_foreward(ctweeny* __self__);
void ctweeny_backward(ctweeny* __self__);
ctweeny_Direction_Enum ctweeny_direction(ctweeny *__self__);

#endif

