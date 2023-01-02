#include "stm32f10x.h"

#define ENCODER_EXTI_Trigger EXTI_Trigger_Falling					// 触发方式
#define ENCODER_PP 1												// 抢占优先级
#define ENCODER_SP 1												// A口响应优先级（B口为A口的响应优先级+1）

#define ENCODER_A_GPIOx GPIOB
#define ENCODER_A_GPIO_Pin GPIO_Pin_1
#define ENCODER_A_GPIO_PortSource GPIO_PortSourceGPIOB
#define ENCODER_A_GPIO_PinSource GPIO_PinSource1
#define ENCODER_A_EXTI_Linex EXTI_Line0								// 中断线（请手动适配中断函数名称）
#define ENCODER_A_NVIC_IRQChannel EXTI0_IRQn						// 中断通道

#define ENCODER_B_GPIOx GPIOB
#define ENCODER_B_GPIO_Pin GPIO_Pin_0
#define ENCODER_B_GPIO_PortSource GPIO_PortSourceGPIOB
#define ENCODER_B_GPIO_PinSource GPIO_PinSource0
#define ENCODER_B_EXTI_Linex EXTI_Line1								// 中断线（请手动适配中断函数名称）
#define ENCODER_B_NVIC_IRQChannel EXTI1_IRQn						// 中断通道

int32_t Encoder_Count = 0;

void Encoder_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);					// AFIO属于APB2. 开启AFIO的时钟

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = ENCODER_A_GPIO_Pin;
	GPIO_Init(ENCODER_A_GPIOx, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = ENCODER_B_GPIO_Pin;
	GPIO_Init(ENCODER_B_GPIOx, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(ENCODER_A_GPIO_PortSource, ENCODER_A_GPIO_PinSource);
	GPIO_EXTILineConfig(ENCODER_B_GPIO_PortSource, ENCODER_B_GPIO_PinSource);
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = ENCODER_A_EXTI_Linex | ENCODER_B_EXTI_Linex;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = ENCODER_EXTI_Trigger;					// 触发方式
	EXTI_Init(&EXTI_InitStructure);
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);						// 注意：分组方式整个芯片只能用一种。最好放在main那里
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = ENCODER_A_NVIC_IRQChannel;			// 选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							// 中断通道是使能还是失能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ENCODER_PP;		// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ENCODER_SP;				// 响应优先级
	NVIC_Init(&NVIC_InitStructure);
	NVIC_InitStructure.NVIC_IRQChannel = ENCODER_B_NVIC_IRQChannel;			// 选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;							// 中断通道是使能还是失能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = ENCODER_PP;		// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = ENCODER_SP + 1;			// 响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

int32_t Encoder_Get(void){
	return Encoder_Count;
}

// 中断函数
void EXTI0_IREHandler(void){
	// 只是让中断的模式固定所以判断一下
	if (EXTI_GetITStatus(ENCODER_A_EXTI_Linex) == SET){
		if (GPIO_ReadInputDataBit(ENCODER_B_GPIOx, ENCODER_B_GPIO_Pin) == 0){
			Encoder_Count--;
		}
		EXTI_ClearITPendingBit(ENCODER_A_EXTI_Linex);
	}
}

void EXTI1_IREHandler(void){
	if (EXTI_GetITStatus(ENCODER_B_EXTI_Linex) == SET){
		if (GPIO_ReadInputDataBit(ENCODER_A_GPIOx, ENCODER_A_GPIO_Pin) == 0){
			Encoder_Count++;
		}
		EXTI_ClearITPendingBit(ENCODER_B_EXTI_Linex);
	}
}
