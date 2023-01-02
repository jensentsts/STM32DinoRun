#ifndef __LED144_H__
#define __LED144_H__

//#include "LCD144_Font_ASCII.h"
//#include "LCD144_Font_GBK.h"
#include "stm32f10x.h"                  // Device header

//#define LCD144_USE_LANDSCAPE          // 设置水平显示
//#define LCD144_BUFFER_MODE              // 使用缓冲区

#define LCD144_BLACK 0x0000
#define LCD144_WHITE 0xFFFF
#define LCD144_RED 0xF800
#define LCD144_GREEN 0x0FE0
#define LCD144_BLUE  0x001F
#define LCD144_YELLOW 0xFFE0
#define LCD144_GRAY0 0xEF7D   	        //灰色0 3165 00110 001011 00101
#define LCD144_GRAY1 0x8410             //灰色1 00000 000000 00000
#define LCD144_GRAY2 0x4208      	    //灰色2 1111111111011111

#define LCD144_LTY 0x667F               // 洛天依
#define LCD144_YH  0x9FFF               // 言和
#define LCD144_YZL 0xECC0               // 乐正绫

uint16_t LCD144_RGB888toRGB565(uint32_t RGB888Color);
uint32_t LCD144_RGB565toRGB888(uint32_t RGB565Color);
void LCD144_Init(void);
void LCD144_Clear(void);
void LCD144_SetRegion(uint8_t XStart, uint8_t YStart, uint8_t XEnd, uint8_t YEnd);

void LCD144_ShowChar(uint8_t X, uint8_t Y, char Char, uint16_t fc, uint16_t bc);
void LCD144_ShowString(uint8_t X, uint8_t Y, char *Str, uint16_t fc, uint16_t bc);
void LCD144_ShowNum(uint8_t X, uint8_t Y, uint32_t Num, uint8_t Length, uint16_t fc, uint16_t bc);
void LCD144_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Num, uint8_t Length, uint16_t fc, uint16_t bc);

#ifdef __LCD144_FONT_GBK_H__
void LCD144_ShowGBK(uint8_t x, uint8_t y, uint8_t *s, uint16_t fc, uint16_t bc);
void LCD144_ShowGBKString(uint8_t X, uint8_t Y, uint8_t *Str, uint16_t fc, uint16_t bc);
#endif

void LCD144_Dot(uint8_t X, uint8_t Y, uint16_t Color);
void LCD144_Line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint16_t Color);
void LCD144_Square(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t Fill, uint16_t Color);
void LCD144_Circle(uint8_t X, uint8_t Y, uint8_t Radius, uint16_t Color);
void LCD144_ShowGraph(uint8_t X, uint8_t Y, uint16_t *Graph, uint8_t Width, uint8_t Height);

#endif
