/******************************************************************
* @�ļ�  : dht11.c
* @����  : caochao
* @����  : 19-9-2017
* @ժҪ  : ���C�ļ�ʱʵ��hdt11�ĸ������ƺ���
* �޸��� :
*  ����  :
*
* �޸��� :
*  ����  :
*******************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include "dht11.h"
#include "delay.h"
#include "usart.h"

/*******************DHT11�ڲ��ӿں���*****************************/
static void     DHT11_GPIO_IN(void);    
static void     DHT11_GPIO_OUT(void);    
static void     DHT11_Reset(void); 
static uint8_t  DHT11_Read_Bit(void);
static uint8_t  DHT11_Read_Byte(void); 

/****************************************************************
*   ��������DHT11_Reset(void)
*   ��  �ܣ���λDHT11
*   ��  �룺
*          ��
*   ��  ����
*          ��
****************************************************************/
void DHT11_Reset(void)
{
    DHT11_GPIO_OUT();       //DQ��Ϊ���ģʽ
    DHT11_HIGH;              //��������
    DHT11_LOW;              //���� DQ
    DHT11_Delay1ms(30);     //�������� 18ms
    DHT11_HIGH;             //����DQ
    DHT11_Delay1us(30);     //�������� 20~40us
}

/********************************************************************
*   ��������DHT11_Check
*   ��  �ܣ����DHT11�Ƿ����
*   ��  �룺
*          ��
*   ��  ����
*          1�� ��ʾDHT11������
*          0�� ��ʾDHT11����
*********************************************************************/
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    
    DHT11_GPIO_IN();     //DQΪ����ģʽ
    while ((DHT11_READ == 0x01) && (retry < 100))  //�ȴ�DQ��Ϊ�͵�ƽ
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    if (retry >= 100)
    {
        return 1;
    }

    retry = 0;
    while ((DHT11_READ == 0x00) && (retry < 100))  //�ȴ�DQ��Ϊ�ߵ�ƽ
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    if (retry >= 100)
    {
        return 1;
    }

    return 0; 
}

/************************************************************
*   ��������DHT11_Read_Bit(void)
*   ��  �ܣ���DTH11��һλ����
*   ��  �룺
*          ��
*   ��  ����
*          1����ʾDHT11����ߵ�ƽ
*          0����ʾDHT11����͵�ƽ
***************************************************************/
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    
    while ((DHT11_READ== 0x01) && (retry<100))  //�ȴ���Ϊ�͵�ƽ
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    retry = 0;
    while((DHT11_READ == 0x00) && (retry<100))  //�ȴ���ߵ�ƽ
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    DHT11_Delay1us(32);        //�ȴ� 32us
    if (DHT11_READ == 0x01)
    {
        return 1;
    }
    else
    {      
        return 0;
    }
}

/****************************************************************
*   ��������DHT11_Read_Byte
*   ��  �ܣ���DTH11���ֽ�����
*   ��  �룺
*           ��
*   ��  ���� 
*          ���� ������һ���ֽ�����
*****************************************************************/
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i;
    uint8_t dat = 0;

    DHT11_GPIO_IN();              //DQΪ����ģʽ 
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

/****************************************************************
*      ��������DHT11_Read_All
*        ���ܣ���DTH11�����ݲ���ʾ����
*        ���룺
*              ��
*        �����
*              0: ��ʾ�����ݳɹ�
*              1����ʾDHT11������
*              2����ʾDHT11����У�����
*****************************************************************/
uint8_t DHT11_Read_Data(float *temp,float *humi)
{
    uint8_t i;
    uint8_t buf[5];
    int16_t tempData;
    
    DHT11_Reset();   //DHT11��λ
    if (DHT11_Check() == 0x01)
    {
       return 1;
    }

    for(i = 0; i < 5; i++)      //��ȡ 40 λ����
    {
        buf[i] = DHT11_Read_Byte();
    }
    
    if( (buf[0]+buf[1]+buf[2]+buf[3]) == buf[4] )
    {
        tempData = (int16_t)(((uint16_t)buf[2]<<8) | buf[3]);
        *temp = tempData*1.0/256;
        tempData = (int16_t)(((uint16_t)buf[0]<<8) | buf[1]);
        *humi = tempData*1.0/256;
        
        return 0;
    }
    else
    {
        return 2;
    }    
}

/******************************************************************
*   ��������DHT11_GPIO_OUT(void)
*   ��  �ܣ���DQ�����ó����ģʽ
*   ��  �룺
*          ��
*   ��  ����
*          ��
*******************************************************************/  
void DHT11_GPIO_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DHT11_PERIPH,ENABLE);//��DHT11��ʱ��  
    
    GPIO_InitStructure.GPIO_Pin	= DHT11_PIN;   //GPIOG.10Ϊ�����������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    
    GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
}    

/**********************************************************************
*   ��������DHT11_GPIO_IN(void)
*   ��  �ܣ���DQ�����ó�����ģʽ
*   ��  �룺
*          ��
*   ��  ����
*          ��
**********************************************************************/
void DHT11_GPIO_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DHT11_PERIPH,ENABLE);//��DHT11��ʱ��  
    
    GPIO_InitStructure.GPIO_Pin	= DHT11_PIN;     //GPIOG.10Ϊ�����������ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
}
/*******************************�ļ�����********************************/



