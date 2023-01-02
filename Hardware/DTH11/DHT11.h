/******************************************************************
* @�ļ�  : dht11.h
* @����  : caochao
* @����  : 19-9-2018
* @ժҪ  : ���h�ļ�ʱ��DHT11��������غ궨�������
*
* �޸��� :
*  ����  :
*
* �޸��� :
*  ����  :
******************************************************************/
/* �����ֹ�ݹ�İ��� */
#ifndef _DHT11_H
#define _DHT11_H

#ifdef __cplusplus
extern "C"{
#endif
  
//----------------DHT11��I/O�궨��-------------------------------//
//#define DHT11_PORT     GPIOG
#define DHT11_PORT 	   GPIOA
#define DHT11_PIN      GPIO_Pin_8
#define DHT11_PERIPH   RCC_APB2Periph_GPIOA

//-----------------�����ʱ����---------------------------------//
/*DHT11ʱ����Ҫ��ʱ���� Ϊ����ֲ��Ҫ����ṩ��ʱ����
void Delay1ms(uint16_t nms) void Delay1us(uint32_t nus)*/
#define  DHT11_Delay1us       Delay_us
#define  DHT11_Delay1ms       Delay_ms
         
#define DHT11_HIGH     GPIO_SetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_LOW      GPIO_ResetBits(DHT11_PORT,DHT11_PIN)
#define DHT11_READ     GPIO_ReadInputDataBit(DHT11_PORT,DHT11_PIN)

//----------------DHT11��������-----------------------------------//
uint8_t DHT11_Read_Data(float *temp,float *humi);
      
#ifdef __cplusplus
}
#endif

#endif /*dht11.h*/

/************************�ļ�����***********************************/


