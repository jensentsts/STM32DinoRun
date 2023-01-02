#include "stm32f10x.h"                  // Device header

unsigned long int RandSeed = 1;

int rand(void){
    RandSeed = RandSeed*1103515245 + 12345;
    return (unsigned int)(RandSeed/65536)%32768;
}

void srand(unsigned int seed){
    RandSeed = seed;
}

/**
  * @brief	快速交换两个字节
  * @param	*a: 一个值
  * @param	*b: 一个值
  * @retval	无
  */
void QuickSwap(int8_t* a, int8_t* b){
	*a ^= *b;
	*b ^= *a;
	*a ^= *b;
}
