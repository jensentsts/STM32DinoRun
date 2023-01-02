#include "stm32f10x.h" // Device header
#include <stdio.h>
#include <stdarg.h>

void Serial_Init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;			// 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;			// 上拉输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
 	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 9600;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART1, &USART_InitStructure);
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStructure);
	
	USART_Cmd(USART1, ENABLE);
}

void Serial_SendByte(uint8_t Byte){
	USART_SendData(USART1, Byte);
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}

void Serial_SendArray(uint8_t *arr, uint32_t Length){
	while (Length--){
		Serial_SendByte(*arr);
		arr++;
	}
}

void Serial_SendStr(uint8_t *str){
	while (*str != '\0'){
		Serial_SendByte(*str);
		str++;
	}
}

void Serial_SendNum(uint32_t Number){
	uint8_t buffer[11] = {0};
	int8_t length = 0;
	int8_t i;
	do{
		buffer[length++] = Number % 10 + '0';
		Number /= 10;
	}while (Number != 0);
	for (i = length - 1; i >= 0; --i){
		Serial_SendByte(buffer[i]);
	}
}

// for printf
int fputc(int ch, FILE *f){
	Serial_SendByte(ch);
	return ch;
}

void Serial_Printf(char *format, ...){
	char buffer[128];
	va_list arg;
	va_start(arg, format);
	vsprintf(buffer, format, arg);
	va_end(arg);
	Serial_SendStr((uint8_t*)buffer);
}
