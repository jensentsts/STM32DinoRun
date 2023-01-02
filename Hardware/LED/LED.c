#include "stm32f10x.h"                  // Device header
#include "BasicStructs.h"

//////////////////////////////////////////

#define LED_CONFIG_LISTMAX 10			// LED信息最大容纳量（取值<=65535）
#define LED_CONFIG_GPIOMODE GPIO_Mode_Out_PP// 默认的GPIO模式（可以在Key_Add_Mode()的第三个参数中对特定接口作特定的设置）

//////////////////////////////////////////

#define LED_TARGETED_LED LED_List[index]

PeriphData LED_List[LED_CONFIG_LISTMAX];// 存储LED引脚信息
uint8_t LED_ListSize;					// LED_List的大小

/**
  * @brief	初始化
  * @param	无
  * @retval	无
  */
void LED_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	LED_ListSize = 0;
}

/** 
  * @brief	点亮LED
  * @param	index 灯的编号
  * @retval	无
  */
void LED_On(uint8_t index){
	if (--index >= LED_ListSize)
		return;
	GPIO_ResetBits(LED_TARGETED_LED.GPIOX, LED_TARGETED_LED.PinNum);
}

/** 
  * @brief	熄灭LED
  * @param	index 灯的编号
  * @retval	无
  */
void LED_Off(uint8_t index){
	if (--index >= LED_ListSize)
		return;
	GPIO_SetBits(LED_TARGETED_LED.GPIOX, LED_TARGETED_LED.PinNum);
}

/**
  * @brief	初始化LED所需要的引脚
  * @param	index 灯的编号
  * @retval	无
  */
void LED_PinInit(uint8_t index){
	if (--index >= LED_ListSize)
		return;
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = LED_TARGETED_LED.PinNum;
	GPIO_InitStructure.GPIO_Mode = LED_TARGETED_LED.Mode;
	GPIO_Init(LED_TARGETED_LED.GPIOX, &GPIO_InitStructure);
	LED_Off(index + 1);
}

/**
  * @brief	添加LED信息
  * @param	LED的GPIO引脚信息
  * @retval	如果添加失败（超过最大容纳量），则返回0xff；否则返回灯的编号
  */
uint8_t LED_Add(GPIO_TypeDef* GPIOX, uint16_t PinNum){
	if (LED_ListSize == LED_CONFIG_LISTMAX)
		return 0xff;
	
	PeriphData LED_InitStructure;
	LED_InitStructure.GPIOX = GPIOX;
	LED_InitStructure.PinNum = PinNum;
	LED_InitStructure.Mode = LED_CONFIG_GPIOMODE;
	LED_List[++LED_ListSize - 1] = LED_InitStructure;
	LED_PinInit(LED_ListSize);
	return LED_ListSize;
}

/**
  * @brief	添加LED信息
  * @param	LED的GPIO引脚信息
  * @retval	如果添加失败（超过最大容纳量），则返回0xff；否则返回灯的编号
  */
uint8_t LED_Add_Mode(GPIO_TypeDef* GPIOX, uint16_t PinNum, GPIOMode_TypeDef Mode){
	if (LED_ListSize == LED_CONFIG_LISTMAX)
		return 0xff;
	
	PeriphData LED_InitStructure;
	LED_InitStructure.GPIOX = GPIOX;
	LED_InitStructure.PinNum = PinNum;
	LED_InitStructure.Mode = Mode;
	LED_List[++LED_ListSize - 1] = LED_InitStructure;
	LED_PinInit(LED_ListSize);
	return LED_ListSize;
}

/**
  * @brief	查询系统里面有多少个LED灯的信息
  * @param	无
  * @retval	当前系统里面有多少个LED灯的信息
  */
uint8_t LED_Size(void){
	return LED_ListSize;
}

/**
  * @brief	翻转LED亮灭状态
  * @param	index LED的索引
  * @retval	无
  */

#include "OLED.h"

void LED_Turn(uint8_t index){
	if (--index >= LED_ListSize)
		return;
	
	GPIO_WriteBit(LED_TARGETED_LED.GPIOX, LED_TARGETED_LED.PinNum, GPIO_ReadOutputDataBit(LED_TARGETED_LED.GPIOX, LED_TARGETED_LED.PinNum) == Bit_SET ? Bit_RESET : Bit_SET);
}

//////////////////////////
// 下面这个空行不要删掉 //
//////////////////////////
