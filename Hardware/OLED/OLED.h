#ifndef __OLED_H__
#define __OLED_H__
#include "stm32f10x.h"                  // Device header

#define OLED_BUFFER_MODE				// 定义启用输出缓冲
#define OLED_BUFFER_CLEAR				// 是否在每次Refresh后清除buffer中的内容

#define OLED_SCREEN_WIDTH 128
#define OLED_SCREEN_HEIGHT 64

void OLED_Init(void);
void OLED_ClearScreen(void);

// 当做终端进行输出

#ifndef OLED_BUFFER_MODE
void OLED_Shell_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_Shell_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_Shell_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_Shell_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_Shell_ShowHex(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_Shell_ShowBin(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
#endif

#ifdef OLED_BUFFER_MODE
/* OLED屏上的颜色的枚举 */
typedef enum{
	BLACK = 0,							// 黑色
	WHITE = 1							// 白色
}OLED_ColorTypeDef;

void OLED_ClearBuffer(void);
void OLED_Refresh(void);

void OLED_Dot(uint8_t ScreenX, uint8_t ScreenY, OLED_ColorTypeDef Color);
void OLED_Line(uint8_t ScreenX1, uint8_t ScreenY1, uint8_t ScreenX2, uint8_t ScreenY2, OLED_ColorTypeDef Color);
void OLED_Square(uint8_t ScreenX1, uint8_t ScreenY1, uint8_t ScreenX2, uint8_t ScreenY2, uint8_t Fill, OLED_ColorTypeDef Color);
void OLED_FilletMatrix(uint8_t ScreenX1, uint8_t ScreenY1, uint8_t ScreenX2, uint8_t ScreenY2, uint8_t radius, uint8_t Fill, OLED_ColorTypeDef Color);
void OLED_Circle(uint8_t ScreenX, uint8_t ScreenY, uint16_t radius, uint8_t Fill, OLED_ColorTypeDef Color);		// TODO：实现填充圆的绘制（目前：画一个普通的空白的圆形）

void OLED_ShowChar(uint8_t ScreenX, uint8_t ScreenY, char Char, OLED_ColorTypeDef Color);
void OLED_ShowString(uint8_t ScreenX, uint8_t ScreenY, char *String, OLED_ColorTypeDef Color);
void OLED_ShowNum(uint8_t ScreenX, uint8_t ScreenY, uint32_t Number, uint8_t Length, OLED_ColorTypeDef Color);
void OLED_ShowSignedNum(uint8_t ScreenX, uint8_t ScreenY, int32_t Number, uint8_t Length, OLED_ColorTypeDef Color);
void OLED_ShowHex(uint8_t ScreenX, uint8_t ScreenY, uint32_t Number, uint8_t Length, OLED_ColorTypeDef Color);
void OLED_ShowBin(uint8_t ScreenX, uint8_t ScreenY, uint32_t Number, uint8_t Length, OLED_ColorTypeDef Color);

void OLED_WritePage(uint8_t ScreenX, uint8_t ScreenY, uint8_t pageData);
uint8_t OLED_ReadPage(uint8_t ScreenX, uint8_t ScreenY);
void OLED_Graph(uint8_t ScreenX, uint8_t ScreenY, uint8_t Width, uint8_t Height, uint8_t *Graph);

void OLED_WriteCommand(uint8_t Command);
void OLED_WriteData(uint8_t Data);
void OLED_SetCursor(uint8_t OLED_X, uint8_t OLED_Y);
#endif	// OLED_BUFFER_MODE

#endif	// __OLED_H__
