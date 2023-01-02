#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "stm32f10x.h" // Device header

void Key_Init(void);
uint8_t Key_Add(GPIO_TypeDef* GPIOX, uint16_t PinNum);
uint8_t Key_Add_Mode(GPIO_TypeDef* GPIOX, uint16_t PinNum, GPIOMode_TypeDef Mode);
uint8_t Key_GetNum(void);
uint8_t Key_IsPressed(uint8_t index);
uint8_t Key_OnPressing(uint8_t index);
uint8_t Key_Size(void);

#endif
