#include "stm32f10x.h"                  // Device header
#include "BasicStructs.h"
#include "delay.h"

//////////////////////////////////////////

#define KEY_CONFIG_LISTMAX 10			// 按键信息最大容纳量（取值<=65535）
#define KEY_CONFIG_GPIOMODE GPIO_Mode_IPU// 默认的GPIO模式（可以在Key_Add_Mode()的第三个参数中对特定接口作特定的设置）

//////////////////////////////////////////

#define KEY_TARGETED_KEY Key_List[index]

PeriphData Key_List[KEY_CONFIG_LISTMAX];// 存储按键引脚信息
uint8_t Key_ListSize;					// Key_List的大小

/**
  * @brief	初始化系统
  * @param	无
  * @retval	无
  */
void Key_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	Key_ListSize = 0;
}

/**
  * @brief	初始化按键所需要的引脚
  * @param	index 灯的编号
  * @retval	无
  */
void Key_PinInit(uint8_t index){
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = KEY_TARGETED_KEY.Mode;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = KEY_TARGETED_KEY.PinNum;
	GPIO_Init(KEY_TARGETED_KEY.GPIOX, &GPIO_InitStructure);
}

/**
  * @brief	添加按键信息
  * @param	按键的GPIO引脚信息
  * @retval	如果添加失败（超过最大容纳量），则返回0xff；否则返回灯的编号
  */
uint8_t Key_Add(GPIO_TypeDef* GPIOX, uint16_t PinNum){
	if (Key_ListSize == KEY_CONFIG_LISTMAX)
		return 0xff;
	
	PeriphData Key_InitStructure;
	Key_InitStructure.GPIOX = GPIOX;
	Key_InitStructure.PinNum = PinNum;
	Key_InitStructure.Mode = KEY_CONFIG_GPIOMODE;
	Key_List[++Key_ListSize - 1] = Key_InitStructure;
	Key_PinInit(Key_ListSize - 1);
	return Key_ListSize;
}

/**
  * @brief	添加按键信息，包含驱动类型
  * @param	按键的GPIO引脚信息
  * @retval	如果添加失败（超过最大容纳量），则返回0xff；否则返回灯的编号
  */
uint8_t Key_Add_Mode(GPIO_TypeDef* GPIOX, uint16_t PinNum, GPIOMode_TypeDef Mode){
	if (Key_ListSize == KEY_CONFIG_LISTMAX)
		return 0xff;
	
	PeriphData Key_InitStructure;
	Key_InitStructure.GPIOX = GPIOX;
	Key_InitStructure.PinNum = PinNum;
	Key_InitStructure.Mode = Mode;
	Key_List[++Key_ListSize - 1] = Key_InitStructure;
	Key_PinInit(Key_ListSize - 1);
	return Key_ListSize;
}

/**
  * @brief	获取按键是否按下
  * @param	无
  * @retval	返回按键的编号。如果未曾添加过信息，则返回0xff；如果无按键按下，返回0
  */
uint8_t Button_GetNum(void){
	if (Key_ListSize == 0)
		return 0xff;
	
	uint8_t i;
	for (i = 0; i < Key_ListSize; ++i){
		if (GPIO_ReadInputDataBit(Key_List[i].GPIOX, Key_List[i].PinNum) == 0){
			Delay_ms(20);
			while (GPIO_ReadInputDataBit(Key_List[i].GPIOX, Key_List[i].PinNum) == 0);
			Delay_ms(20);
			return i + 1;
		}
	}
	return 0;
}

/**
  * @brief	获取某按键是否按下
  * @param	index: 按键的编号s
  * @retval 如果index超过最大编号，则返回0xff；如果调用此函数时已已被按下，返回1；否则返回0
  */
uint8_t Key_IsPressed(uint8_t index){
	if (--index >= Key_ListSize)
		return 0xff;
	if (GPIO_ReadInputDataBit(KEY_TARGETED_KEY.GPIOX, KEY_TARGETED_KEY.PinNum) == (KEY_TARGETED_KEY.Mode == GPIO_Mode_IPD)){
		Delay_ms(20);
		while (GPIO_ReadInputDataBit(KEY_TARGETED_KEY.GPIOX, KEY_TARGETED_KEY.PinNum) == (KEY_TARGETED_KEY.Mode == GPIO_Mode_IPD));
		Delay_ms(20);
		return 1;
	}
	return 0;
}

/**
  * @brief	获取某按键是否被按下且未被释放
  * @param	index: 按键的编号
  * @retval 如果index超过最大编号，则返回0xff；如果调用此函数时已已被按下，返回1；否则返回0。
  */
uint8_t Key_OnPressing(uint8_t index){
	if (--index >= Key_ListSize)
		return 0xff;
	if (GPIO_ReadInputDataBit(KEY_TARGETED_KEY.GPIOX, KEY_TARGETED_KEY.PinNum) == (KEY_TARGETED_KEY.Mode == GPIO_Mode_IPD)){
		return 1;
	}
	return 0;
}

/**
  * @brief	获取有多少个按键信息在系统中
  * @param	无
  * @retval	系统所拥有的按键信息的数量
  */
uint8_t Key_Size(void){
	return Key_ListSize;
}

// TODO: 实现Key_Edit(GPIO_TypeDef GPIOX, uint16_t pin);
