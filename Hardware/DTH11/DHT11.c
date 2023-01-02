/******************************************************************
* @文件  : dht11.c
* @作者  : caochao
* @日期  : 19-9-2017
* @摘要  : 这个C文件时实现hdt11的各个控制函数
* 修改人 :
*  日期  :
*
* 修改人 :
*  日期  :
*******************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include "dht11.h"
#include "delay.h"
#include "usart.h"

/*******************DHT11内部接口函数*****************************/
static void     DHT11_GPIO_IN(void);    
static void     DHT11_GPIO_OUT(void);    
static void     DHT11_Reset(void); 
static uint8_t  DHT11_Read_Bit(void);
static uint8_t  DHT11_Read_Byte(void); 

/****************************************************************
*   函数名：DHT11_Reset(void)
*   功  能：复位DHT11
*   输  入：
*          无
*   输  出：
*          无
****************************************************************/
void DHT11_Reset(void)
{
    DHT11_GPIO_OUT();       //DQ线为输出模式
    DHT11_HIGH;              //首先拉高
    DHT11_LOW;              //拉低 DQ
    DHT11_Delay1ms(30);     //拉低至少 18ms
    DHT11_HIGH;             //拉高DQ
    DHT11_Delay1us(30);     //主机拉高 20~40us
}

/********************************************************************
*   函数名：DHT11_Check
*   功  能：检查DHT11是否存在
*   输  入：
*          无
*   输  出：
*          1： 表示DHT11不存在
*          0： 表示DHT11存在
*********************************************************************/
uint8_t DHT11_Check(void)
{
    uint8_t retry = 0;
    
    DHT11_GPIO_IN();     //DQ为输入模式
    while ((DHT11_READ == 0x01) && (retry < 100))  //等待DQ变为低电平
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    if (retry >= 100)
    {
        return 1;
    }

    retry = 0;
    while ((DHT11_READ == 0x00) && (retry < 100))  //等待DQ变为高电平
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
*   函数名：DHT11_Read_Bit(void)
*   功  能：从DTH11读一位数据
*   输  入：
*          无
*   输  出：
*          1：表示DHT11输出高电平
*          0：表示DHT11输出低电平
***************************************************************/
uint8_t DHT11_Read_Bit(void)
{
    uint8_t retry = 0;
    
    while ((DHT11_READ== 0x01) && (retry<100))  //等待变为低电平
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    retry = 0;
    while((DHT11_READ == 0x00) && (retry<100))  //等待变高电平
    {
        retry++;
        DHT11_Delay1us(1);
    }
    
    DHT11_Delay1us(32);        //等待 32us
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
*   函数名：DHT11_Read_Byte
*   功  能：从DTH11读字节数据
*   输  入：
*           无
*   输  出： 
*          返回 读到的一个字节数据
*****************************************************************/
uint8_t DHT11_Read_Byte(void)
{
    uint8_t i;
    uint8_t dat = 0;

    DHT11_GPIO_IN();              //DQ为输入模式 
    for (i = 0; i < 8; i++)
    {
        dat <<= 1;
        dat |= DHT11_Read_Bit();
    }
    return dat;
}

/****************************************************************
*      函数名：DHT11_Read_All
*        功能：从DTH11读数据并显示数据
*        输入：
*              无
*        输出：
*              0: 表示读数据成功
*              1：表示DHT11不存在
*              2：表示DHT11数据校验错误
*****************************************************************/
uint8_t DHT11_Read_Data(float *temp,float *humi)
{
    uint8_t i;
    uint8_t buf[5];
    int16_t tempData;
    
    DHT11_Reset();   //DHT11复位
    if (DHT11_Check() == 0x01)
    {
       return 1;
    }

    for(i = 0; i < 5; i++)      //读取 40 位数据
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
*   函数名：DHT11_GPIO_OUT(void)
*   功  能：把DQ线设置成输出模式
*   输  入：
*          无
*   输  出：
*          无
*******************************************************************/  
void DHT11_GPIO_OUT(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DHT11_PERIPH,ENABLE);//打开DHT11的时钟  
    
    GPIO_InitStructure.GPIO_Pin	= DHT11_PIN;   //GPIOG.10为复用推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    
    GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
}    

/**********************************************************************
*   函数名：DHT11_GPIO_IN(void)
*   功  能：把DQ线设置成输入模式
*   输  入：
*          无
*   输  出：
*          无
**********************************************************************/
void DHT11_GPIO_IN(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(DHT11_PERIPH,ENABLE);//打开DHT11的时钟  
    
    GPIO_InitStructure.GPIO_Pin	= DHT11_PIN;     //GPIOG.10为复用推挽输出模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	
	GPIO_Init(DHT11_PORT,&GPIO_InitStructure);
}
/*******************************文件结束********************************/



