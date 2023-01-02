/**
  * @note 本项目编译环境：AC5 -cpp11
  */
#include "stm32f10x.h" // Device header
#include "arduiduidui.h"
#include "misc.h"
#include "Serial.h"
#include <stdio.h>

int fps;

void Timer_Init(void){
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	// 定时器初始化
	TIM_InternalClockConfig(TIM2);									// 选择内部时钟（定时器上电后默认用内部时钟,可省略）
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;		// 指定时钟分频 - 滤波器的参数 && 信号延迟 && 极性
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;	// 计数器模式（向上计数）
	TIM_TimeBaseInitStructure.TIM_Period = 10000 - 1;				// 自动重装器的值（“周期”），-1由公式得来
	TIM_TimeBaseInitStructure.TIM_Prescaler = 7200 - 1;				// 预分频器，-1由公式得来（在10KHz下记1w个数）
	TIM_TimeBaseInitStructure.TIM_RepetitionCounter = 0;			// 重复计数器
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);							// 手动清除更新中断标志位

	// 使能中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);						// 开启更新中断到NVIC的通路
	// NVIC
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;					// 选择中断通道
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;					// 中断通道是使能还是失能
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;		// 抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;				// 响应优先级
	NVIC_Init(&NVIC_InitStructure);
	// 启动定时器
	TIM_Cmd(TIM2, ENABLE);
}

// 中断函数
void TIM2_IRQHandler(void){
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) == SET){
		printf("FPS: %d\r\n", fps);
		fps = 0;
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	Serial_Init();
	Timer_Init();
	setup();
	while (1){
		loop();
		++fps;
	}
	return 0;
}
