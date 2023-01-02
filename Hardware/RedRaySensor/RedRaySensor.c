#include "stm32f10x.h"

uint16_t CounterSensor_Count = 0;

void CounterSensor_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);			// AFIO属于APB2. 开启AFIO的时钟
	// EXTI和NVIC的时钟一直开着
	// RCC位于内核之外，管不到NVIC
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);	// PB14
	
	EXTI_InitTypeDef EXTI_InitStructure;
	EXTI_InitStructure.EXTI_Line = EXTI_Line14;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;			// 下降沿触发
	EXTI_Init(&EXTI_InitStructure);
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// 注意：分组方式整个芯片只能用一种。最好放在main那里
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			// 选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 中断通道是使能还是失能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;		// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;				// 响应优先级
	NVIC_Init(&NVIC_InitStructure);
}

uint16_t CountSensor_Get(void){
	return CounterSensor_Count;
}

// 中断函数名不能写错
// 建议去startup_stm32f10x_md.s启动文件里复制。
void EXTI15_10_IRQHandler(void){
	// 中断标志位判断，确保是想要的中断进入此函数
	if (EXTI_GetITStatus(EXTI_Line14) == SET){
		CounterSensor_Count++;
		// 中断程序结束后要手动清除标志位
		EXTI_ClearITPendingBit(EXTI_Line14);
	}
}
