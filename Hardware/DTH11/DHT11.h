/******************************************************************
* @文件  : dht11.h
* @作者  : caochao
* @日期  : 19-9-2018
* @摘要  : 这个h文件时定DHT11函数和相关宏定义的声明
*
* 修改人 :
*  日期  :
*
* 修改人 :
*  日期  :
******************************************************************/
/* 定义防止递归的包容 */
#ifndef _DHT11_H
#define _DHT11_H

#ifdef __cplusplus
extern "C"{
#endif
  
//----------------DHT11的I/O宏定义-------------------------------//
//#define DHT11_PORT     GPIOG
#define DHT11_PORT 	   GPIOA
#define DHT11_PIN      GPIO_Pin_8
#define DHT11_PERIPH   RCC_APB2Periph_GPIOA

//-----------------外界延时函数---------------------------------//
/*DHT11时序需要延时函数 为了移植需要外界提供延时函数
void Delay1ms(uint16_t nms) void Delay1us(uint32_t nus)*/
#define  DHT11_Delay1us       Delay_us
#define  DHT11_Delay1ms       Delay_ms
         
#define DHT11_HIGH     GPIO_SetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_LOW      GPIO_ResetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_READ     GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN)

//----------------DHT11操作函数-----------------------------------//
uint8_t DHT11_Read_Data(float *temp,float *humi);
      
#ifdef __cplusplus
}
#endif

#endif /*dht11.h*/

/************************文件结束***********************************/


