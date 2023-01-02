#ifndef __LED_H__
#define __LED_H__

#include "stm32f10x.h"                  // Device header

void LED_Init(void);
void LED_Add(GPIO_TypeDef* GPIOX, uint16_t PinNum);
void LED_Add_Mode(GPIO_TypeDef* GPIOX, uint16_t PinNum, GPIOMode_TypeDef Mode);
void LED_On(uint8_t index);
void LED_Off(uint8_t index);
uint8_t LED_Size(void);
void LED_Turn(uint8_t index);

#endif
