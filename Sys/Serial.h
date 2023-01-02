#ifndef __SERIAL_H
#define __SERIAL_H

void Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *arr, uint32_t Length);
void Serial_SendStr(uint8_t *str);
void Serial_SendNum(uint32_t Number);
void Serial_Printf(char *format, ...);

#endif
