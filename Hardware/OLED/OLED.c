#include "stm32f10x.h"                  // Device header
#include "misc.h"
#include "OLED_Font.h"
#include "OLED.h"

#define OLED_W 128						// 宽度
#define OLED_H 64						// 高度

/***************************************************************/

#ifdef __STC89C5xRC_RDP_H__

sbit OLED_SCL = P0^1;					// SCL
sbit OLED_SDA = P0^0;					// SDA

#define OLED_SCL_w(x) OLED_SCL = x;
#define OLED_SDA_w(x) OLED_SDA = x;

#endif

#ifdef __STM32F10x_H

#define OLED_RCC RCC_APB2Periph_GPIOB

#define OLED_GPIOx_SCL GPIOB
#define OLED_Pin_SCL GPIO_Pin_10
#define OLED_GPIOx_SDA GPIOB
#define OLED_Pin_SDA GPIO_Pin_11

#define OLED_SCL_w(x) GPIO_WriteBit(OLED_GPIOx_SCL, OLED_Pin_SCL, (BitAction)(x));
#define OLED_SDA_w(x) GPIO_WriteBit(OLED_GPIOx_SDA, OLED_Pin_SDA, (BitAction)(x));

#endif

/***************************************************************/

#ifdef OLED_BUFFER_MODE

uint8_t OLED_Buffer[OLED_W][OLED_H / 8] = {0};
uint8_t OLED_BufferLineUpdFlag = 0;				// 行更新标志，用于记录 buffer 中的各行是否更新，按位存储，从低位到高位每一位的值与从第一行到第八行每一行的更新情况一一对应。
uint8_t OLED_BufferUpdLagFlag = 0;				// 如果某一行在上一次Refresh中更新了，那么它会记录标志，记录规则与 OLED_BufferLineUpdFlag 一致，并在下一次Refresh()时清楚此标志

#endif	// OLED_BUFFER_MODE

void OLED_I2C_Init(void){
	#ifdef __STM32F10x_H
    RCC_APB2PeriphClockCmd(OLED_RCC, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = OLED_Pin_SCL;
 	GPIO_Init(OLED_GPIOx_SCL, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = OLED_Pin_SDA;
 	GPIO_Init(OLED_GPIOx_SDA, &GPIO_InitStructure);
	#endif
	
	OLED_SDA_w(1);
	OLED_SCL_w(1);
}

void OLED_I2C_Start(void){
	OLED_SDA_w(1);
	OLED_SCL_w(1);
	OLED_SDA_w(0);
	OLED_SCL_w(0);
}

void OLED_I2C_Stop(void){
	OLED_SDA_w(0);
	OLED_SCL_w(1);
	OLED_SDA_w(1);
}

void OLED_I2C_SendByte(uint8_t Byte){
	uint8_t i;
	for (i = 0; i < 8; ++i){
		OLED_SDA_w(Byte & (0x80 >> i));
		OLED_SCL_w(1);
		OLED_SCL_w(0);
	}
	OLED_SCL_w(1);
	OLED_SCL_w(0);
}

void OLED_WriteCommand(uint8_t Cmd){
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// 从机地址
	OLED_I2C_SendByte(0x00);		// 写命令
	OLED_I2C_SendByte(Cmd);			// 发送命令
	OLED_I2C_Stop();
}

void OLED_WriteData(uint8_t Data){
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// 从机地址
	OLED_I2C_SendByte(0x40);		// 写数据
	OLED_I2C_SendByte(Data);		// 发送数据
	OLED_I2C_Stop();
}

void OLED_SetCursor(uint8_t X, uint8_t Y){
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		// 从机地址
	OLED_I2C_SendByte(0x00);		// 写命令
	OLED_I2C_SendByte(0xB0 | Y);	// Y坐标
	OLED_I2C_SendByte(0x10 | ((X & 0xF0) >> 4));	// 高八位
	OLED_I2C_SendByte(0x00 | (X & 0x0F));			// 低八位
	OLED_I2C_Stop();
}

void OLED_ClearScreen(void){
	uint8_t i, j;
	for (j = 0; j < OLED_H / 8; ++j){
		OLED_SetCursor(0, j);
		OLED_I2C_Start();
		OLED_I2C_SendByte(0x78);	// 从机地址
		OLED_I2C_SendByte(0x40);	// 写数据
		for (i = 0; i < OLED_W; ++i){
			OLED_I2C_SendByte(0x00);
		}
		OLED_I2C_Stop();
	}
}

#ifdef OLED_BUFFER_MODE
void OLED_ClearBuffer(void){
	uint8_t i, j;
	OLED_BufferLineUpdFlag = 0;
	for (j = 0; j < 8; ++j){
		OLED_SetCursor(0, j);
		for (i = 0; i < OLED_W; ++i){
			OLED_Buffer[i][j] = 0;
		}
	}
}

void OLED_Refresh(void){
	uint8_t i, j;
	uint8_t OLED_BufferUpdFlag = OLED_BufferUpdLagFlag | OLED_BufferLineUpdFlag;
	for (j = 0; j < 8; ++j){
		if (OLED_BufferUpdFlag & 1){
			OLED_SetCursor(0, j);
			OLED_I2C_Start();
			OLED_I2C_SendByte(0x78);		// 从机地址
			OLED_I2C_SendByte(0x40);		// 写数据
			for (i = 0; i < 128; ++i){
				OLED_I2C_SendByte(OLED_Buffer[i][j]);
#ifdef OLED_BUFFER_CLEAR
				OLED_Buffer[i][j] = 0;
#endif // OLED_BUFFER_CLEAR
			}
		}
		OLED_I2C_Stop();
		OLED_BufferUpdFlag >>= 1;
	}
	OLED_BufferUpdLagFlag = OLED_BufferLineUpdFlag;
}
#endif // OLED_BUFFER_MODE

void OLED_Init(void)
{
	uint32_t i, j;
	
	//上电延时
	for (i = 0; i < 1000; i++) {
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_ClearScreen();			//OLED清屏

#ifdef OLED_BUFFER_MODE

	OLED_ClearBuffer();
	OLED_BufferLineUpdFlag = 0;
	
#endif
}

void OLED_Shell_ShowChar(uint8_t Line, uint8_t Column, char Char){
	uint8_t i;
#ifdef OLED_BUFFER_MODE
	for (i = 0; i < 8; ++i){
		// 上半部分内容
		OLED_Buffer[(Column - 1) * 8 + i][(Line - 1) * 2] = OLED_F8x16[Char - ' '][i];
	}
	for (i = 0; i < 8; ++i){
		// 下半部分内容
		OLED_Buffer[(Column - 1) * 8 + i][(Line - 1) * 2 + 1] = OLED_F8x16[Char - ' '][i + 8];
	}
#else
	OLED_SetCursor((Column - 1) * 8, (Line - 1) * 2);		//设置光标位置在上半部分
	for (i = 0; i < 8; ++i){
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Column - 1) * 8, (Line - 1) * 2 + 1);	//设置光标位置在下半部分
	for (i = 0; i < 8; ++i){
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
#endif
}

void OLED_Shell_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; ++i){
		OLED_Shell_ShowChar(Line, Column + i, String[i]);
	}
}

uint32_t OLED_Pow(uint32_t base, uint32_t expo){
	uint32_t res = 1;
	while (expo != 0){
		if (expo & 1){
			res *= base;
		}
		expo >>= 1;
		base *= base;
	}
	return res;
}

void OLED_Shell_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length){
	while (Length--){
		OLED_Shell_ShowChar(Line, Column + Length, Number % 10 + '0');
		Number /= 10;
	}
}

void OLED_Shell_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length){
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0){
		OLED_Shell_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else{
		OLED_Shell_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; ++i){
		OLED_Shell_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

void OLED_Shell_ShowHex(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; ++i){
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10){
			OLED_Shell_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else{
			OLED_Shell_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

void OLED_Shell_ShowBin(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length){
	while (Length--){
		OLED_Shell_ShowChar(Line, Column + Length, Number & 1 + '0');
		Number >>= 1;
	}
}

#ifdef OLED_BUFFER_MODE

void OLED_Dot(uint8_t ScreenX, uint8_t ScreenY, OLED_ColorTypeDef Color){
	uint8_t XIndex = ScreenX,
			YIndex = ScreenY / 8,
			YOffset = ScreenY % 8;
	uint8_t *Color_Frame = &OLED_Buffer[XIndex][YIndex];

	if (ScreenY >= OLED_H || ScreenX >= OLED_W){
		return;
	}

	/*uint8_t Color_FrameCondition = (*Color_Frame) & (1 << YOffset),
			Color_FrameAddition = Color << YOffset;
	*Color_Frame ^= Color_FrameCondition ^ Color_FrameAddition;*/
	*Color_Frame ^= ((*Color_Frame) & (1 << YOffset)) ^ (Color << YOffset);
	OLED_BufferLineUpdFlag |= 1 << YIndex;
}

void OLED_WritePage(uint8_t ScreenX, uint8_t ScreenY, uint8_t pageData){
	uint8_t XIndex = ScreenX,
			YIndex = ScreenY / 8,
			YOffset = ScreenY % 8;
	uint8_t *Color_Frame = &OLED_Buffer[XIndex][YIndex];

	if (ScreenY >= OLED_H || ScreenX >= OLED_W){
		return;
	}

	*Color_Frame ^= (*Color_Frame) & (0xFF << YOffset);
	*Color_Frame |= pageData << YOffset;
	OLED_BufferLineUpdFlag |= 1 << YIndex;

	if (YIndex + 1 < OLED_H){
		*(Color_Frame + 1) ^= (*(Color_Frame + 1)) & (0xFF >> (8 - YOffset));
		*(Color_Frame + 1) |= pageData >> (8 - YOffset);
		OLED_BufferLineUpdFlag |= 1 << (YIndex + 1);
	}
}

uint8_t OLED_ReadPage(uint8_t ScreenX, uint8_t ScreenY){
	uint8_t XIndex = ScreenX,
			YIndex = ScreenY / 8,
			YOffset = ScreenY % 8;
	uint8_t *Color_Frame = &OLED_Buffer[XIndex][YIndex];
	
	uint8_t res = 0;

	if (ScreenY >= OLED_H || ScreenX >= OLED_W){
		return 0;
	}

	res = (*Color_Frame) & (0xFF <<  YOffset);
	
	if (YIndex + 1 < OLED_H){
		res |= (*(Color_Frame + 1)) & (0xFF >> (8 - YOffset));
	}

	return res;
}


void OLED_ShowChar(uint8_t ScreenX, uint8_t ScreenY, char Char, OLED_ColorTypeDef Color){
	uint8_t i, 
			fontWriteStyle = ~((uint8_t)0x00 - (uint8_t)Color);
	// 通过 OLED_F8x16[][] ^ fontWriteStyle 实现字体颜色的功能
	// 若 Color == WHITE，则有 OLED_F8x16[][] ^ 0x00，得到的子模为原本的子模
	// 若 Color == BLACK，则有 OLED_F8x16[][] ^ 0xFF，得到的子模为取反后的子模
	for (i = 0; i < 8; ++i){
		OLED_WritePage(ScreenX + i, ScreenY, OLED_F8x16[Char - ' '][i] ^ fontWriteStyle);
		OLED_WritePage(ScreenX + i, ScreenY + 8, OLED_F8x16[Char - ' '][i + 8] ^ fontWriteStyle);
	}
}

void OLED_ShowString(uint8_t ScreenX, uint8_t ScreenY, char *String, OLED_ColorTypeDef Color){
	uint8_t i;
	for (i = 0; String[i] != '\0'; ++i){
		OLED_ShowChar(ScreenX + i * 8, ScreenY, String[i], Color);
	}
}

void OLED_ShowNum(uint8_t ScreenX, uint8_t ScreenY, uint32_t Number, uint8_t Length, OLED_ColorTypeDef Color){
	while(Length--){
		OLED_ShowChar(ScreenX + Length * 8, ScreenY, (Number % 10) + '0', Color);
		Number /= 10;
	};
}

void OLED_ShowSignedNum(uint8_t ScreenX, uint8_t ScreenY, int32_t Number, uint8_t Length, OLED_ColorTypeDef Color){
	uint8_t i;
	uint32_t shownNumber;
	if (Number >= 0){
		OLED_ShowChar(ScreenX, ScreenY, '+', Color);
		shownNumber = Number;
	}
	else{
		OLED_ShowChar(ScreenX, ScreenY, '-', Color);
		shownNumber = -ScreenX;
	}

	for (i = Length - 1; i >= Length; i--){
		OLED_ShowChar(ScreenX + i + 8, ScreenY, shownNumber % 10 + '0', Color);
		shownNumber /= 10;
	}
}

void OLED_ShowHex(uint8_t ScreenX, uint8_t ScreenY, uint32_t Number, uint8_t Length, OLED_ColorTypeDef Color)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; ++i){
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10){
			OLED_ShowChar(ScreenX + i * 8, ScreenY, SingleNumber + '0', Color);
		}
		else{
			OLED_ShowChar(ScreenX + i * 8, ScreenY, SingleNumber - 10 + 'A', Color);
		}
	}
}

void OLED_ShowBin(uint8_t ScreenX, uint8_t ScreenY, uint32_t Number, uint8_t Length, OLED_ColorTypeDef Color){
	while (Length--){
		OLED_ShowChar(ScreenX + Length * 8, ScreenY, Number & 1 + '0', Color);
		Number >>= 1;
	}
}

void OLED_Line(uint8_t ScreenX1, uint8_t ScreenY1, uint8_t ScreenX2, uint8_t ScreenY2, OLED_ColorTypeDef Color){
	uint8_t di;
	uint8_t dx;
	uint8_t dy;
	uint8_t gx;
	uint8_t gy;
	di = 0;
	gx = (ScreenX2 > ScreenX1) ? 1 : -1;
	gy = (ScreenY2 > ScreenY1) ? 1 : -1;
	dx = (ScreenX2 - ScreenX1) * gx + 1;
	dy = (ScreenY2 - ScreenY1) * gy + 1;
	if(dx >= dy){
		for(/*ScreenX2 += gx*/; ScreenX1 != ScreenX2; ScreenX1 += gx){
			di += dy;
			if(di <= dx){
				OLED_Dot(ScreenX1, ScreenY1, Color);
				continue;
			}
			while((di > dx) && (ScreenY1 != ScreenY2)){
				di -= dx;
				ScreenY1 += gy;
				OLED_Dot(ScreenX1, ScreenY1, Color);
			}
		}
	}
	else{
		for(/*ScreenY2 += gw*/; ScreenY1 != ScreenY2; ScreenY1 += gy){
			di += dx;
			if(di <= dy){
				OLED_Dot(ScreenX1, ScreenY1, Color);
				continue;
			}
			while((di > dy) && (ScreenX1 != ScreenX2)){
				di -= dy;
				ScreenX1 += gx;
				OLED_Dot(ScreenX1, ScreenY1, Color);
			}
		}
	}
}

// 没有 (ScreenX1, ScreenY1) -> (ScreenX2, ScreenY2)
void OLED_Square(uint8_t ScreenX1, uint8_t ScreenY1, uint8_t ScreenX2, uint8_t ScreenY2, uint8_t Fill, OLED_ColorTypeDef Color){
	uint16_t i, j;
	uint8_t pageData = (uint8_t)0x00 - Color,
			YOffset = 8 - ((ScreenY2 - ScreenY1) % 8);
	if (ScreenX1 > ScreenX2 || ScreenY1 > ScreenY2){
		return;
	}
	if (Fill == 1){
		for (i = ScreenX1; i <= ScreenX2; ++i){
			for (j = ScreenY1; j <= ScreenY2 - 8; j += 8){
				OLED_WritePage(i, j, pageData);
			}
			OLED_WritePage(i, j, pageData >> YOffset);
		}
		return;
	}
	OLED_Line(ScreenX1, ScreenY1, ScreenX1, ScreenY2, Color);
	OLED_Line(ScreenX1, ScreenY1, ScreenX2, ScreenY1, Color);
	OLED_Line(ScreenX2, ScreenY2, ScreenX1, ScreenY2, Color);
	OLED_Line(ScreenX2, ScreenY2, ScreenX2, ScreenY1, Color);
}


void OLED_FilletMatrix(uint8_t ScreenX1, uint8_t ScreenY1, uint8_t ScreenX2, uint8_t ScreenY2, uint8_t Radius, uint8_t Fill, OLED_ColorTypeDef Color){
	int xi;
	int yi;
	int di;
	if (ScreenX1 > ScreenX2 || ScreenY1 > ScreenY2){
		return;
	}
	// limit the radius within (ScreenY2 - ScreenY1) / 2
	if (Radius > (ScreenY2 - ScreenY1) / 2){
		Radius = (ScreenY2 - ScreenY1) / 2;
	}
	// if filled
	if (Fill == 1){
		OLED_Square(ScreenX1 + Radius, ScreenY1, ScreenX2 - Radius, ScreenY1 + Radius, 1, Color);
		OLED_Square(ScreenX1, ScreenY1 + Radius, ScreenX2, ScreenY2 - Radius, 1, Color);
		OLED_Square(ScreenX1 + Radius, ScreenY2 - Radius, ScreenX2 - Radius, ScreenY2, 1, Color);
		OLED_Circle(ScreenX1 + Radius, ScreenY1 + Radius, Radius, 1, Color);
		OLED_Circle(ScreenX1 + Radius, ScreenY2 - Radius, Radius, 1, Color);
		OLED_Circle(ScreenX2 - Radius, ScreenY1 + Radius, Radius, 1, Color);
		OLED_Circle(ScreenX2 - Radius, ScreenY2 - Radius, Radius, 1, Color);
		return;
	}
	// if not filled
	OLED_Line(ScreenX1, ScreenY1 + Radius, ScreenX1, ScreenY2 - Radius, Color);
	OLED_Line(ScreenX1 + Radius, ScreenY1, ScreenX2 - Radius, ScreenY1, Color);
	OLED_Line(ScreenX2 - Radius, ScreenY2, ScreenX1 + Radius, ScreenY2, Color);
	OLED_Line(ScreenX2, ScreenY2 - Radius, ScreenX2, ScreenY1 + Radius, Color);
	di = 0 - (Radius >> 1);
	xi = 0;
	yi = Radius;
	while(yi >= xi){
		OLED_Dot(ScreenX2 - Radius + yi - 1, ScreenY1 + Radius - xi, Color);
		OLED_Dot(ScreenX2 - Radius + xi - 1, ScreenY1 + Radius - yi, Color);
		OLED_Dot(ScreenX1 + Radius - xi, ScreenY1 + Radius - yi, Color);
		OLED_Dot(ScreenX1 + Radius - yi, ScreenY1 + Radius - xi, Color);
		OLED_Dot(ScreenX1 + Radius - yi, ScreenY2 - Radius + xi - 1, Color);
		OLED_Dot(ScreenX1 + Radius - xi, ScreenY2 - Radius + yi - 1, Color);
		OLED_Dot(ScreenX2 - Radius + xi - 1, ScreenY2 - Radius + yi - 1, Color);
		OLED_Dot(ScreenX2 - Radius + yi - 1, ScreenY2 - Radius + xi - 1, Color);
		xi++;
		if (di < 0){
			di += xi;
		}
		else{
			yi--;
			di += xi - yi;
		}
	}
	
}

void OLED_Circle(uint8_t ScreenX, uint8_t ScreenY, uint16_t Radius, uint8_t Fill, OLED_ColorTypeDef Color){
	int xi;
	int yi;
	int di;
	di = 0 - (Radius >> 1);
	xi = 0;
	yi = Radius;
	while(yi >= xi){
		OLED_Dot(ScreenX + yi - 1, ScreenY + xi - 1, Color);
		OLED_Dot(ScreenX + xi - 1, ScreenY + yi - 1, Color);
		OLED_Dot(ScreenX - xi, ScreenY - yi, Color);
		OLED_Dot(ScreenX - yi, ScreenY - xi, Color);
		OLED_Dot(ScreenX - xi, ScreenY + yi - 1, Color);
		OLED_Dot(ScreenX - yi, ScreenY + xi - 1, Color);
		OLED_Dot(ScreenX + xi - 1, ScreenY - yi, Color);
		OLED_Dot(ScreenX + yi - 1, ScreenY - xi, Color);
		xi++;
		if (di < 0){
			di += xi;
		}
		else{
			yi--;
			di += xi - yi;
		}
	}
}

void OLED_Graph(uint8_t ScreenX, uint8_t ScreenY, uint8_t Width, uint8_t Height, uint8_t *Graph){
	uint8_t i, j;
	uint8_t YOffset = 8 - (Height % 8);
	uint16_t index = 0;
	for (i = 0; i < Width; ++i){
		for (j = 0; j < Height - 8; j += 8){
			OLED_WritePage(i, j, Graph[index]);
			index++;
		}
		OLED_WritePage(i, j, OLED_ReadPage(i, j) | (Graph[index] >> YOffset));
		index++;
	}
}

#endif
