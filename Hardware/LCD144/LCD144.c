#include "stm32f10x.h"                  // Device header
#include "LCD144.h"
#include "LCD144_Font_ASCII.h"

#define LCD144_W 128
#define LCD144_H 128

#ifdef __STC89C5xRC_RDP_H__
sbit LCD144_BLK       =P1^0;           // 接模块BLK引脚，背光可以采用IO控制或者PWM控制，也可以直接接到高电平常亮
sbit LCD144_CS        =P1^1;           // 接模块CE引脚，接裸屏Pin12_CS
sbit LCD144_DC        =P1^2;           // 接模块D/C引脚，接裸屏Pin7_A0
sbit LCD144_RES       =P1^3;           // 接模块RST引脚，接裸屏Pin6_RES
sbit LCD144_SDA       =P1^4;           // 接模块DIN/MOSI引脚，接裸屏Pin8_SDA
sbit LCD144_SCL       =P1^5;           // 接模块CLK引脚,接裸屏Pin9_SCL

#define LCD144_SCL_w(x) LCD144_SCL = x;
#define LCD144_SDA_w(x) LCD144_SDA = x;
#define LCD144_RES_w(x) LCD144_SCL = x;
#define LCD144_DC_w(x)  LCD144_SDA = x;
#define LCD144_CS_w(x)  LCD144_SCL = x;
#define LCD144_BLK_w(x) LCD144_SDA = x;
#endif

#ifdef __STM32F10x_H
#define LCD144_RCC RCC_APB2Periph_GPIOB

#define LCD144_GPIOX_SCL GPIOB
#define LCD144_PIN_SCL GPIO_Pin_9
#define LCD144_GPIOX_SDA GPIOB
#define LCD144_PIN_SDA GPIO_Pin_8
#define LCD144_GPIOX_RES GPIOB
#define LCD144_PIN_RES GPIO_Pin_7
#define LCD144_GPIOX_DC GPIOB
#define LCD144_PIN_DC GPIO_Pin_6
#define LCD144_GPIOX_CS GPIOB
#define LCD144_PIN_CS GPIO_Pin_5
#define LCD144_GPIOX_BLK GPIOB
#define LCD144_PIN_BLK GPIO_Pin_4

#define LCD144_SCL_w(x) GPIO_WriteBit(LCD144_GPIOX_SCL, LCD144_PIN_SCL, (BitAction)(x));
#define LCD144_SDA_w(x) GPIO_WriteBit(LCD144_GPIOX_SDA, LCD144_PIN_SDA, (BitAction)(x));
#define LCD144_RES_w(x) GPIO_WriteBit(LCD144_GPIOX_RES, LCD144_PIN_RES, (BitAction)(x));
#define LCD144_DC_w(x) GPIO_WriteBit(LCD144_GPIOX_DC, LCD144_PIN_DC, (BitAction)(x));
#define LCD144_CS_w(x) GPIO_WriteBit(LCD144_GPIOX_CS, LCD144_PIN_CS, (BitAction)(x));
#define LCD144_BLK_w(x) GPIO_WriteBit(LCD144_GPIOX_BLK, LCD144_PIN_BLK, (BitAction)(x));
#endif

#ifdef LCD144_BUFFER_MODE
#define LCD144_BLOCKUPD_STD 12                      // 将输出平均分为若干8*8的小区块
                                                    // 若区块中需要输出新信息的像素点数量多于此，
                                                    // 则将以整个区块的形式绘制屏幕
                                                    // 否则，分别绘制其中的更新的点
const uint16_t LCD144_Buffer[LCD144_W][LCD144_H];         // 缓冲区，第二页用于写入新的帧，第一页保存上次输出的帧
const uint8_t LCD144_UpdFlag[LCD144_W / 8][LCD144_H];
#endif

void LCD144_Delayms(uint32_t time){
	uint32_t i,j;
	for(i=0;i<time;i++)
		for(j=0;j<250;j++);
}

void LCD144_SPI_Send8Byte(uint8_t Data){
    int16_t i = 0;
    for (i = 8; i > 0; --i){
        LCD144_SDA_w((Data & 0x80) >> 7);
        LCD144_SCL_w(0);
        LCD144_SCL_w(1);
        Data <<= 1;
    }
}

void LCD144_WriteCmd(uint8_t Cmd){
    LCD144_CS_w(0);
    LCD144_DC_w(0);
    LCD144_SPI_Send8Byte(Cmd);
    LCD144_CS_w(0);
}

void LCD144_WriteData(uint8_t Data){
    LCD144_CS_w(0);
    LCD144_DC_w(1);
    LCD144_SPI_Send8Byte(Data);
    LCD144_CS_w(0);
}

void LCD144_WriteData_16Bit(uint16_t Data){
    LCD144_CS_w(0);
    LCD144_DC_w(1);
    LCD144_SPI_Send8Byte(Data >> 8);     // 高八位
    LCD144_SPI_Send8Byte(Data);          // 低八位
    LCD144_CS_w(1);
}

void LCD144_Reset(){
    LCD144_RES_w(0);
    LCD144_Delayms(100);
    LCD144_RES_w(1);
    LCD144_Delayms(100);
}

#ifdef LCD144_BUFFER_MODE
void LCD144_WriteBuffer(uint8_t X, uint8_t Y, uint16_t Color){
    if (X >= LCD144_W || Y >= LCD144_H)
        return;
    LCD144_Buffer[X][Y] = Color;
    LCD144_UpdFlag[X / 8][Y] |= 1 << (X % 8);
}
#endif

void LCD144_Clear(void){
    uint8_t i, j;
    LCD144_SetRegion(0, 0, LCD144_W - 1, LCD144_H - 1);
    for (i = 0; i < LCD144_H; ++i){
        for (j = 0; j < LCD144_W; ++j){
            LCD144_WriteData_16Bit(0x0000);

#ifdef LCD144_BUFFER_MODE
            LCD144_WriteBuffer(i, j, 0x0000);
#endif

        }
    }
}

void LCD144_Init(void){

#ifdef __STM32F10x_H
	
    RCC_APB2PeriphClockCmd(LCD144_RCC, ENABLE);
    GPIO_InitTypeDef  GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin = LCD144_PIN_SCL;
	GPIO_Init(LCD144_GPIOX_SCL, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LCD144_PIN_SDA;
    GPIO_Init(LCD144_GPIOX_SDA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LCD144_PIN_RES;
    GPIO_Init(LCD144_GPIOX_RES, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LCD144_PIN_CS;
    GPIO_Init(LCD144_GPIOX_CS, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LCD144_PIN_DC;
	GPIO_Init(LCD144_GPIOX_DC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = LCD144_PIN_BLK;
    GPIO_Init(LCD144_GPIOX_BLK, &GPIO_InitStructure);

#endif

    LCD144_Reset();                      //Reset before LCD Init.
		
	//LCD Init For 1.44Inch LCD Panel with ST7735R.
	LCD144_WriteCmd(0x11);               //Sleep exit 
	LCD144_Delayms (120);
		
	//ST7735R Frame Rate
	LCD144_WriteCmd(0xB1); 
	LCD144_WriteData(0x01); 
	LCD144_WriteData(0x2C); 
	LCD144_WriteData(0x2D); 

	LCD144_WriteCmd(0xB2); 
	LCD144_WriteData(0x01); 
	LCD144_WriteData(0x2C); 
	LCD144_WriteData(0x2D); 

	LCD144_WriteCmd(0xB3); 
	LCD144_WriteData(0x01); 
	LCD144_WriteData(0x2C); 
	LCD144_WriteData(0x2D); 
	LCD144_WriteData(0x01); 
	LCD144_WriteData(0x2C); 
	LCD144_WriteData(0x2D); 
	
	LCD144_WriteCmd(0xB4);               //Column inversion 
	LCD144_WriteData(0x07); 
	
	//ST7735R Power Sequence
	LCD144_WriteCmd(0xC0); 
	LCD144_WriteData(0xA2); 
	LCD144_WriteData(0x02); 
	LCD144_WriteData(0x84); 
	LCD144_WriteCmd(0xC1); 
	LCD144_WriteData(0xC5); 

	LCD144_WriteCmd(0xC2); 
	LCD144_WriteData(0x0A); 
	LCD144_WriteData(0x00); 

	LCD144_WriteCmd(0xC3); 
	LCD144_WriteData(0x8A); 
	LCD144_WriteData(0x2A); 
	LCD144_WriteCmd(0xC4); 
	LCD144_WriteData(0x8A); 
	LCD144_WriteData(0xEE); 
	
	LCD144_WriteCmd(0xC5);               //VCOM 
	LCD144_WriteData(0x0E); 
	
	LCD144_WriteCmd(0x36);               //MX, MY, RGB mode 
#ifdef LCD144_USE_LANDSCAPE              // 使用竖屏模式
	LCD144_WriteData(0xA8);              //竖屏C8 横屏08 A8
#else
	LCD144_WriteData(0xC8);              //竖屏C8 横屏08 A8
#endif		
	//ST7735R Gamma Sequence
	LCD144_WriteCmd(0xe0); 
	LCD144_WriteData(0x0f); 
	LCD144_WriteData(0x1a); 
	LCD144_WriteData(0x0f); 
	LCD144_WriteData(0x18); 
	LCD144_WriteData(0x2f); 
	LCD144_WriteData(0x28); 
	LCD144_WriteData(0x20); 
	LCD144_WriteData(0x22); 
	LCD144_WriteData(0x1f); 
	LCD144_WriteData(0x1b); 
	LCD144_WriteData(0x23); 
	LCD144_WriteData(0x37); 
	LCD144_WriteData(0x00); 	
	LCD144_WriteData(0x07); 
	LCD144_WriteData(0x02); 
	LCD144_WriteData(0x10); 

	LCD144_WriteCmd(0xe1); 
	LCD144_WriteData(0x0f); 
	LCD144_WriteData(0x1b); 
	LCD144_WriteData(0x0f); 
	LCD144_WriteData(0x17); 
	LCD144_WriteData(0x33); 
	LCD144_WriteData(0x2c); 
	LCD144_WriteData(0x29); 
	LCD144_WriteData(0x2e); 
	LCD144_WriteData(0x30); 
	LCD144_WriteData(0x30); 
	LCD144_WriteData(0x39); 
	LCD144_WriteData(0x3f); 
	LCD144_WriteData(0x00); 
	LCD144_WriteData(0x07); 
	LCD144_WriteData(0x03); 
	LCD144_WriteData(0x10);  
	
	LCD144_WriteCmd(0x2a);
	LCD144_WriteData(0x00);
    LCD144_WriteData(0x00 + 2);
    LCD144_WriteData(0x00);
    LCD144_WriteData(0x80 + 2);

    LCD144_WriteCmd(0x2b);
	LCD144_WriteData(0x00);
    LCD144_WriteData(0x00 + 3);
    LCD144_WriteData(0x00);
    LCD144_WriteData(0x80 + 3);

    LCD144_WriteCmd(0xF0);               //Enable test command  
	LCD144_WriteData(0x01); 
	LCD144_WriteCmd(0xF6);               //Disable ram power save mode 
	LCD144_WriteData(0x00); 
	
	LCD144_WriteCmd(0x3A);               //65k mode 
	LCD144_WriteData(0x05); 
	
	LCD144_WriteCmd(0x29);               //Display on

	LCD144_Clear();
}

void LCD144_SetRegion(uint8_t XStart, uint8_t YStart, uint8_t XEnd, uint8_t YEnd){

#ifdef LCD144_USE_LANDSPCAE
    
    LCD144_WriteCmd(0x2a);
	LCD144_WriteData(0x00);
	LCD144_WriteData(XStart + 3);
	LCD144_WriteData(0x00);
	LCD144_WriteData(XEnd + 3);

	LCD144_WriteCmd(0x2b);
	LCD144_WriteData(0x00);
	LCD144_WriteData(YStart + 2);
	LCD144_WriteData(0x00);
	LCD144_WriteData(YEnd + 2);

#else//竖屏模式	

	LCD144_WriteCmd(0x2a);
	LCD144_WriteData(0x00);
	LCD144_WriteData(XStart + 2);
	LCD144_WriteData(0x00);
    LCD144_WriteData(XEnd + 2);

    LCD144_WriteCmd(0x2b);
	LCD144_WriteData(0x00);
	LCD144_WriteData(YStart + 3);
	LCD144_WriteData(0x00);
	LCD144_WriteData(YEnd + 3);	

#endif

	LCD144_WriteCmd(0x2c);
}

uint16_t LCD144_RGB888toRGB565(uint32_t RGB888Color){
    uint8_t R = RGB888Color >> 16,
            G = RGB888Color >> 8,
            B = RGB888Color >> 0;
    uint16_t RGB565Color = 0;
    R >>= 3;
    G >>= 2;
    B >>= 3;
    RGB565Color = R << 11 | G << 5 | B << 0;
    return RGB565Color;
}

uint32_t LCD144_RGB565toRGB888(uint32_t RGB565Color){
    uint8_t R = (RGB565Color >> 11) & 0x1F,
            G = (RGB565Color >> 5) & 0x3F,
            B = (RGB565Color >> 0) & 0x1F;
    uint32_t RGB888Color = 0;
    RGB888Color = R << 19 | G << 10 | B << 3;
    return RGB888Color;
}

/*
    * @brief 显示一个8*16的ASCII字符
    * @Param
    *   X: x
    *   Y: y
    *   Char: 英文字母  
    *   fc: 前景色
    *   bc: 背景色
    * @retval 无
    */
void LCD144_ShowChar(uint8_t X, uint8_t Y, char Char, uint16_t fc, uint16_t bc){
    uint8_t i, j, fontCache;

#ifdef LCD144_BUFFER_MODE
    for (i = 0; i < 16; ++i){
        fontCache = LCD144_F8x16[Char - ' '][i];
        for (j = 0; j < 8; ++j){
            LCD144_WriteBuffer(X + j, Y + i, fontCache & 1 ? fc : bc);
            fontCache >>= 1;
        }
    }
#else
    LCD144_SetRegion(X, Y, 
	X + 8 - 1 > LCD144_W - 1 ? LCD144_W - 1 : X + 8 - 1, 
	Y + 16 - 1 > LCD144_H - 1 ? LCD144_H -  1 : Y + 16 - 1);
    LCD144_CS_w(0);
    LCD144_DC_w(1);
    for (i = 0; i < 16 && i + Y < LCD144_H; ++i){
        fontCache = LCD144_F8x16[Char - ' '][i];
        for (j = 0; j < 8 && j + X < LCD144_W; ++j){
            //LCD144_WriteData_16Bit(fontCache & 1 ? fc : bc);
			LCD144_SPI_Send8Byte((fontCache & 1 ? fc : bc) >> 8);     // 高八位
			LCD144_SPI_Send8Byte(fontCache & 1 ? fc : bc);          // 低八位
            fontCache >>= 1;
        }
    }
    LCD144_CS_w(1);
#endif // LCD144_BUFFER_MODE
}

/*
    * @brief 显示ASCII字符串
    * @Param
    *   X: x
    *   Y: y
    *   Str: 字符串
    *   fc: 前景色
    *   bc: 背景色
    * @retval 无
    */
void LCD144_ShowString(uint8_t X, uint8_t Y, char *Str, uint16_t fc, uint16_t bc){
    while (*Str){
        LCD144_ShowChar(X, Y, *Str, fc, bc);
        ++Str;
        X += 8;
    }
}

/*
    * @brief 显示数字(Ascii)
    * @Param
    *   X: x
    *   Y: y
    *   Num: 数字
    *   Length: 数字的位数
    *   fc: 前景色
    *   bc: 背景色
    * @retval 无
    */
void LCD144_ShowNum(uint8_t X, uint8_t Y, uint32_t Num, uint8_t Length, uint16_t fc, uint16_t bc){
    do{
        LCD144_ShowChar(X + Length * 8, Y, (Num % 10) + '0', fc, bc);
        Num /= 10;
    } while (Length--);
}

/*
    * @brief 显示有符号数字(Ascii)
    * @Param
    *   X: x
    *   Y: y
    *   Num: 数字
    *   Length: 数字的位数
    *   fc: 前景色
    *   bc: 背景色
    * @retval 无
    */
void LCD144_ShowSignedNum(uint8_t X, uint8_t Y, int32_t Num, uint8_t Length, uint16_t fc, uint16_t bc){
    if (Num >= 0){
        LCD144_ShowChar(X, Y, '+', fc, bc);
    }
    else{
        LCD144_ShowChar(X, Y, '-', fc, bc);
        Num = -Num;
    }
    LCD144_ShowNum(X + 8, Y, Num, Length, fc, bc);
}

#ifdef __LCD144_FONT_GBK_H__
/*
    * @brief 显示一个中文
    * @Param
    *   X: x
    *   Y: y
    *   s: 中文
    *   fc: 前景色
    *   bc: 背景色
    * @retval 无
    */
void LCD144_ShowGBK(uint8_t x, uint8_t y, uint8_t *s, uint16_t fc, uint16_t bc){
    unsigned char i,j;
    unsigned short k, fontCache, fetcher;
#ifdef LCD144_BUFFER_MODE
    for (k = 0; k < LCD144_GBK16_NUM; k++){            // 此处可优化
        if ((LCD144_CH16x16[k].Index[0] == *(s)) && (LCD144_CH16x16[k].Index[1] == *(s + 1))){
            for (i = 0; i < 16; ++i){
                fetcher = 0x80;
                fontCache = LCD144_CH16x16[k].Code[i << 1];
                for (j = 0; j < 8; ++j){
                    LCD144_WriteBuffer(x + j, y + i, fontCache & fetcher ? fc : bc);
                    fetcher >>= 1;
                }
                fetcher = 0x80;
                fontCache = LCD144_CH16x16[k].Code[i << 1 | 1];
                for (j = 8; j < 16; ++j){
                    LCD144_WriteBuffer(x + j, y + i, fontCache & fetcher ? fc : bc);
                    fetcher >>= 1;
                }
            }
            break;
        }
    }
#else
    LCD144_SetRegion(x, y, x + 16 - 1, y + 16 - 1);
    for (k = 0; k < LCD144_GBK16_NUM; k++){            // 此处可优化
        if ((LCD144_CH16x16[k].Index[0] == *(s)) && (LCD144_CH16x16[k].Index[1] == *(s + 1))){
            for (i = 0; i < 16 * 2; ++i){
                fetcher = 0x80;
                fontCache = LCD144_CH16x16[k].Code[i];
                for (j = 0; j < 8; ++j){
                    LCD144_WriteData_16Bit(fontCache & fetcher ? fc : bc);
                    fetcher >>= 1;
                }
            }
            break;
        }
    }
#endif // LCD144_BUFFER_MODE
}

/*
    * @brief 显示中文（可混合ascii）字符串
    * @Param
    *   X: x
    *   Y: y
    *   str: 字符串
    *   fc: 前景色
    *   bc: 背景色
    * @retval 无
    */
void LCD144_ShowGBKString(uint8_t X, uint8_t Y, uint8_t *Str, uint16_t fc, uint16_t bc){
    while (*Str){
        if (*Str >= 128){
            LCD144_ShowGBK(X, Y, Str, fc, bc);
            Str += 2;
            X += 16;
        }
        else{
            LCD144_ShowChar(X, Y, *Str, fc, bc);
            ++Str;
            X += 8;
        }
    }
}
#endif // __LCD144_FONT_GBK_H__

/*
    * @brief 显示图片
    * @Param
    *   X: x
    *   Y: y
    *   Graph: 图片（RGB565）
    *   Width: 宽度
    *   Height: 高度
    * @retval 无
    */
void LCD144_ShowGraph(uint8_t X, uint8_t Y, uint16_t *Graph, uint8_t Width, uint8_t Height){
    uint8_t i, j;
#ifdef LCD144_BUFFER_MODE
    for (i = 0; i < Height; ++i){
        for (j = 0; j < Width; ++j){
            LCD144_WriteBuffer(X + j, Y + i, *Graph);
            ++Graph;
        }
    }
#else
    LCD144_SetRegion(X, Y, 
	X + Width - 1 > LCD144_W - 1 ? LCD144_W - 1 : X + Width - 1, 
	Y + Height - 1 > LCD144_H - 1 ? LCD144_H - 1 : Y + Height - 1);
	
    LCD144_CS_w(0);
    LCD144_DC_w(1);
    for (i = 0; i < Height && i + Y < LCD144_H; ++i){
        for (j = 0; j < Width && j + X < LCD144_W; ++j){
            //LCD144_WriteData_16Bit(*Graph);
			LCD144_SPI_Send8Byte(*Graph >> 8);     // 高八位
			LCD144_SPI_Send8Byte(*Graph);          // 低八位
            ++Graph;
        }
		for (; j < Width; ++j){
			++Graph;
		}
    }
    LCD144_CS_w(1);
#endif
}

/*
    * @brief 画点
    * @Param
    *   X: x
    *   Y: y
    *   Color: 颜色
    * @retval 无
    */
void LCD144_Dot(uint8_t X, uint8_t Y, uint16_t Color){
#ifdef LCD144_BUFFER_MODE
    LCD144_WriteBuffer(X, Y, Color);
#else
    LCD144_SetRegion(X, Y, X, Y);
    LCD144_WriteData_16Bit(Color);
#endif
}

/*
    * @brief 画线
    * @Param
    *   X1: x1
    *   Y1: y1
    *   X2: x2
    *   Y2: y2
    *   Color: 颜色
    * @retval 无
    */
void LCD144_Line(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint16_t Color){
    int di;
	int dx;
	int dy;
	int gx;
	int gy;
	di = 0;
	gx = (X2 > X1) ? 1 : -1;
	gy = (Y2 > Y1) ? 1 : -1;
	dx = (X2 - X1) * gx + 1;
	dy = (Y2 - Y1) * gy + 1;
	if(dx >= dy){
		for(/*X2 += gx*/; X1 != X2; X1 += gx){
			di += dy;
			if(di <= dx){
				LCD144_Dot(X1, Y1, Color);
				continue;
			}
			while((di > dx) && (Y1 != Y2)){
				di -= dx;
				Y1 += gy;
				LCD144_Dot(X1, Y1, Color);
			}
		}
	}
	else{
		for(/*Y2 += gw*/; Y1 != Y2; Y1 += gy){
			di += dx;
			if(di <= dy){
				LCD144_Dot(X1, Y1, Color);
				continue;
			}
			while((di > dy) && (X1 != X2)){
				di -= dy;
				X1 += gx;
				LCD144_Dot(X1, Y1, Color);
			}
		}
	}
}

/*
    * @brief 画方
    * @Param
    *   X1: x1
    *   Y1: y1
    *   X2: x2
    *   Y2: y2
    *   Fill: 是否填充(1: 填充，0: 不填充)
    *   Color: 颜色
    * @retval 无
    */
void LCD144_Square(uint8_t X1, uint8_t Y1, uint8_t X2, uint8_t Y2, uint8_t Fill, uint16_t Color){
	uint16_t i, j;
	if (Fill == 1){
        LCD144_SetRegion(X1, Y1, X2 - 1, Y2 - 1);
        for (i = Y1; i <= Y2; ++i){
			for (j = X1; j <= X2; ++j){
                LCD144_WriteData_16Bit(Color);
            }
		}
		return;
	}
	LCD144_Line(X1, Y1, X1, Y2, Color);
	LCD144_Line(X1, Y1, X2, Y1, Color);
	LCD144_Line(X2, Y2, X1, Y2, Color);
	LCD144_Line(X2, Y2, X2, Y1, Color);
}

/*
    * @brief 画圆
    * @Param
    *   X: x
    *   Y: y
    *   Radius: 半径
    *   Color: 颜色
    * @retval 无
    */
void LCD144_Circle(uint8_t X, uint8_t Y, uint8_t Radius, uint16_t Color){
	int  xi;
	int  yi;
	int  di;
	di = 0 - (Radius >> 1);
	xi = 0;
	yi = Radius;
	while(yi >= xi){
		LCD144_Dot(X + xi - 1, Y + yi - 1, Color);
		LCD144_Dot(X + yi - 1, Y + xi - 1, Color);
		LCD144_Dot(X - xi, Y + yi - 1, Color);
		LCD144_Dot(X - yi, Y + xi - 1, Color);
		LCD144_Dot(X - xi, Y - yi, Color);
		LCD144_Dot(X - yi, Y - xi, Color);
		LCD144_Dot(X + xi - 1, Y - yi, Color);
		LCD144_Dot(X + yi - 1, Y - xi, Color);
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

#ifdef LCD144_BUFFER_MODE

void LCD144_Refresh(){
    uint8_t xpre, ypre, x, y, xblock, yblock, updcount, i;
    uint16_t updque[2][LCD144_BLOCKUPD_STD];    // 当区块内更新点的数量不足 LCD144_BLOCKUPD_STD 时，
                                                // 点坐标和颜色信息分别保存到此队列的第一页和第二页中，
                                                //      (x保存在低八位，y保存在高八位)
                                                // 并在区块检测完成之后使用此队列绘制屏幕
    // 比对不同像素
    for (yblock = 0; yblock < 16; ++yblock){    // 块绝对坐标
        for (xblock = 0; xblock < 16; ++xblock){
            updcount = 0;
            xpre = xblock << 3;                 // 块内点绝对坐标前缀
            ypre = yblock << 3;
            for (y = 0; y < 8; ++y){            // 块内点相对坐标
                for (x = 0; x < 8; ++x){
                    if (LCD144_UpdFlag[xblock][ypre | y] & 1){
                        // 坐标和颜色入队
                        updque[0][updcount] = x | (y << 8);
                        updque[1][updcount++] = LCD144_Buffer[xpre | x][ypre | y];
                    }
                    LCD144_UpdFlag[xblock][ypre | y] >>= 1;
                    // 前往两重循环之外的区块更新代码
                    if (updcount >= LCD144_BLOCKUPD_STD){
                        break;
                    }
                }
                // 前往两重循环之外的区块更新代码
                if (updcount >= LCD144_BLOCKUPD_STD){
                    break;
                }
            }
            // 绘制更新的点
            if (updcount >= LCD144_BLOCKUPD_STD){
                LCD144_SetRegion(xpre, ypre, xpre | 7, ypre | 7);
                for (y = 0; y < 8; ++y){        // 块内点相对坐标
                    for (x = 0; x < 8; ++x){
                        LCD144_WriteData_16Bit(LCD144_Buffer[xpre | x][ypre | y]);
                        LCD144_Buffer[xpre | x][ypre | y] = 0x0000;
                    }
                    LCD144_UpdFlag[xblock][ypre | y] = 0;
                }
            }
            else{
                for (i = 0; i < updcount; ++i){
                    LCD144_Dot(updque[0][i], updque[0][i] >> 8, updque[1][i]);
                    LCD144_Buffer[xpre | updque[0][i]][ypre | (updque[0][i] >> 8)] = 0x0000;
                }
            }
        }
    }
}

#endif

